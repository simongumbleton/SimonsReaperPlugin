#pragma once
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>    //string and other mapping macros
#include <string>
#include <map>

#include "WaapiFunctions.h"
//#include "PluginWindow.h"


class WwiseConnectionHandler
{
	

public:
	WwiseConnectionHandler();
	~WwiseConnectionHandler();

	struct WwiseObject
	{
		std::map<std::string, std::string> properties;
	};

	CurrentWwiseConnection MyCurrentWwiseConnection;

	bool StartGUI(HINSTANCE &myhInst);

	void handle_GUI_notifications(int message);

	void ConnectToWwise(bool suppressOuputMessages, int port);

	void GetSelectedWwiseObjects(bool suppressOuputMessages);

	void GetChildrenFromSelectedParent(bool suppressOuputMessages);

	void GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results);


private:

	void ReportConnectionError(CurrentWwiseConnection attemptedConnection);

};

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
	void handleUI_B_Connect(int notifCode);


	/////Initialise dialogue boxes
	bool init_ALL_OPTIONS(HWND hwnd);
	bool init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices);
	bool init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices);

	INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};




