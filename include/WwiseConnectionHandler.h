#pragma once
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>    //string and other mapping macros
#include <string>
#include <map>

#include "WaapiFunctions.h"
//#include "PluginWindow.h"

typedef struct WwiseObjectStruct
{
	std::map<std::string, std::string> properties;
} WwiseObject;

struct RenderQueJob
{
	std::string RenderQueFilePath;
	WwiseObject parentWwiseObject;
	std::vector<std::string> RenderQueJobFileList;
	std::string ParentReaperProject;
	bool hasRendered = false;
	bool hasImported = false;
	bool isVoice = false;
	std::string ImportLanguage;
	bool OrigDirMatchesWwise = false;
	std::string userOrigsSubDir;
	

};

class WwiseConnectionHandler
{	

public:
	WwiseConnectionHandler();
	~WwiseConnectionHandler();



	CurrentWwiseConnection MyCurrentWwiseConnection;

	bool StartGUI_Get(HINSTANCE &myhInst);

	bool StartGUI_Transfer(HINSTANCE &myhInst);

	bool handle_GUI_Connect();

	bool ConnectToWwise(bool suppressOuputMessages, int port);

	bool GetSelectedWwiseObjects(bool suppressOuputMessages);

	WwiseObject GetSelectedObject();

	void GetChildrenFromSelectedParent(bool suppressOuputMessages);

	bool GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results);

	bool CreateWwiseObjects(bool suppressOutputMessages, CreateObjectArgs& createArgs, AK::WwiseAuthoringAPI::AkJson::Array& Results);

	bool ImportAudioToWwise(bool suppressOutputMessages, ImportObjectArgs& importArgs, AK::WwiseAuthoringAPI::AkJson::Array& Results);


private:

	void ReportConnectionError(CurrentWwiseConnection attemptedConnection);

};


///////////////////////////////////
/////Plugin Window - Implements Object GET
//////////////////////////////////

class PluginWindow
{
	/////Try this approach from https://www.codeguru.com/cpp/w-d/dislog/win32/article.php/c5073/Creating-a-Reusable-Dialog-Class-without-MFC.htm

public:

	PluginWindow();
	~PluginWindow();

	static HWND m_hWindow;
	static long m_lSaveThis;
	PluginWindow* saveThis;

	int DoModal(void);

	static INT_PTR CALLBACK DialogProcStatic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static WwiseConnectionHandler* parentWwiseConnectionHnd;

	void SetupPluginParent(WwiseConnectionHandler *parent);

	////=============================================================================
	int CreatePluginWindow(HINSTANCE hInst, HINSTANCE, LPSTR, int);

private:

	HWND m_hParent;
	int m_nResId;

	PluginWindow* thisPluginWindow;


	////=============================================================================
	////message processing function declarations


	////non-message function declarations
	////=============================================================================
	void OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl);
	////=============================================================================
	INT_PTR OnInitDlg(const HWND hwnd, LPARAM lParam);
	////=============================================================================
	inline int ErrMsg(const std::string& s);

	/////Handle the various UsI elements
	void handleUI_GetFrom(int notifCode);
	void handleUI_GetSelect(int notifCode);
	void handleUI_GetWhere(int notifCode);
	void handleUI_GetReturnOptions(int notifCode);
	void handleUI_B_Connect();
	void handleUI_B_GO();

	void handleUI_B_WwiseTree();

	/////Initialise dialogue boxes
	bool init_ALL_OPTIONS(HWND hwnd);
	bool init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices);
	bool init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices);

	INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};



///////////////////////////////////
/////Create Import Window - Implements Object Create and Import
//////////////////////////////////

class CreateImportWindow
{
public:
	CreateImportWindow();
	~CreateImportWindow();

	static HWND m_hWindow;
	static long m_lSaveThis;
	CreateImportWindow* saveThis;


	static INT_PTR CALLBACK DialogProcStatic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static WwiseConnectionHandler* parentWwiseConnectionHnd;

	void SetupPluginParent(WwiseConnectionHandler *parent);

	////=============================================================================
	int CreateTransferWindow(HINSTANCE hInst, HINSTANCE, LPSTR, int);

private:

	HWND m_hParent;
	int m_nResId;

	CreateImportWindow* thisPluginWindow;


	////=============================================================================
	////message processing function declarations
	void handleUI_B_Connect();
	void handleUI_B_CreateObject();
	void handleUI_B_GetSelectedParent();
	void handleUI_GetType(int notifCode);
	void handleUI_GetNameConflict(int notifCode);
	void handleUI_RenderImport();

	////non-message function declarations
	////=============================================================================
	void OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl);
	////=============================================================================
	INT_PTR OnInitDlg(const HWND hwnd, LPARAM lParam);
	////=============================================================================
	inline int ErrMsg(const std::string& s);

	/////Handle the various UsI elements


	/////Initialise dialogue boxes
	bool init_ALL_OPTIONS(HWND hwnd);
	bool init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices);
	bool init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices);

	void FillRenderQueList(HWND hwnd);
	void UpdateRenderJob_TreeView(HWND hwnd);
	void HandleUI_SetParentForRenderJob(WwiseObject selectedParent);
	bool GetIsVoice();
	std::string GetLanguage();
	bool GetOrigsDirMatchesWwise();
	std::string GetUserOriginalsSubDir();
	void SetStatusMessageText(std::string message);

	bool UpdateProgressDuringRender(int numJobs);

	bool ImportJobsIntoWwise();
	ImportObjectArgs SetupImportArgs(WwiseObject parent, bool isVoice, std::string ImportLanguage, bool OrigsDirMatchesWwise,std::string userOrigSubDir, std::vector<std::string> ImportFiles);
	bool ImportCurrentRenderJob(ImportObjectArgs curJobImportArgs);

	void backupRenderQueFiles();
	void restoreRenderQueFiles();


	INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


class ReaperRenderObj
{
public:
	ReaperRenderObj();
	~ReaperRenderObj();
	void RenderAllQues();
};
