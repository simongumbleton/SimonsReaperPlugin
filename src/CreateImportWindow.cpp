#pragma once
#include "WwiseConnectionHandler.h"
#include "resource.h"
#include "PluginWindow.h"
#include "SimonsReaperPlugin.h"
#include "ReaperRenderQueParser.h"
#include <filesystem>
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <future>

///Handles to UI elements
HWND tr_buttonConnect;
HWND tr_textConnectionStatus;
HWND tr_B_GetSelectedParent;
HWND tr_s_ImportParentID;
HWND tr_s_ImportParentNameType;
HWND tr_B_CreateObject;
HWND tr_c_CreateType;
std::string s_CreateType;
HWND tr_c_CreateNameConflict;
std::string s_CreateNameConflict;
HWND tr_txt_CreateName;
HWND tr_txt_CreateNotes;
HWND tr_Tree_RenderJobTree;
HWND tr_Progress_Import;
HWND B_RenderImport;
HWND txt_status;
HWND check_IsVoice;
HWND txt_Language;
std::string defaultLanguage = "English(US)";
HWND check_OrigDirMatchWwise;

CreateObjectChoices myCreateChoices;

HWND CreateImportWindow::m_hWindow = NULL;
long CreateImportWindow::m_lSaveThis = 0;
WwiseConnectionHandler* CreateImportWindow::parentWwiseConnectionHnd = NULL;

std::vector<RenderQueJob> GlobalListOfRenderQueJobs;

bool AllDone = false;
std::mutex mtx;
int numOfRendersDone = 0;

//=============================================================================
int CreateImportWindow::CreateTransferWindow(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	INT_PTR success = DialogBox(hInst, MAKEINTRESOURCE(IDD_Transfer), 0, DialogProcStatic);
	//HWND success = CreateDialog(hInst, MAKEINTRESOURCE(IDD_Transfer), 0, DialogProcStatic);
	//ShowWindow(success, SW_SHOW);

	if (success == -1)
	{
		ErrMsg(_T("DialogBox failed."));
	}
	return 0;
}

void CreateImportWindow::SetupPluginParent(WwiseConnectionHandler * parent)
{
	parentWwiseConnectionHnd = parent;
	thisPluginWindow = this;
}

INT_PTR CALLBACK CreateImportWindow::DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//PluginWindow* pThis = (PluginWindow*)GetProp(hwnd, "my_class_data");
	//return pThis ? pThis->realDlgProc(hwnd, uMsg, wParam, lParam) : FALSE;
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		OnCommand(hwnd, LOWORD(wParam), HIWORD(wParam),
			reinterpret_cast<HWND>(lParam));
		return 0;
	}
	case WM_INITDIALOG:
	{
		return OnInitDlg(hwnd, lParam);
	}
	default:
		return FALSE;  //let system deal with msg
	}
}

INT_PTR CreateImportWindow::DialogProcStatic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWindow = hDlg;

	CreateImportWindow *pThis = (CreateImportWindow*)m_lSaveThis;

	return (pThis->DlgProc(hDlg, message, wParam, lParam));
}


CreateImportWindow::CreateImportWindow()
{
	m_lSaveThis = (long)this;
	saveThis = this;
}

CreateImportWindow::~CreateImportWindow()
{
}


//=============================================================================
void CreateImportWindow::OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl)
{
	//handles WM_COMMAND message of the modal dialogbox
	switch (id)
	{
	case ID_B_GO:        //RETURN key pressed or 'GO' button selected				 //MessageBox(NULL, "Ok buttor", _T("DEBUG"), MB_OK | MB_ICONEXCLAMATION);
		break;
	case IDC_B_ConnectWwise:
		handleUI_B_Connect();
		break;
	case IDC_B_CreateObject:
		handleUI_B_CreateObject();
		break;
	case IDC_B_GetSelectedParent:
		handleUI_B_GetSelectedParent();
		break;
	case IDC_C_Create_Type:
		handleUI_GetType(notifycode);
		break;
	case IDC_C_CreateOnNameConflict:
		handleUI_GetNameConflict(notifycode);
		break;
	case IDC_B_RenderImport:
		handleUI_RenderImport();
		break;
	case ID_B_CANCEL:    //ESC key pressed or 'cancel' button selected
		m_hWindow = NULL;
		EndDialog(hwnd, id);
		
	}
}
//=============================================================================
INT_PTR CreateImportWindow::OnInitDlg(const HWND hwnd, LPARAM lParam)
{
	//set the small icon for the dialog. IDI_APPLICATION icon is set by default 
	//for winxp
	SendMessage(hwnd, WM_SETICON, ICON_SMALL,
		reinterpret_cast<LPARAM>(LoadImage(0, IDI_APPLICATION, IMAGE_ICON,
			0, 0, LR_SHARED)));
	//ensure focus rectangle is properly draw around control with focus
	PostMessage(hwnd, WM_KEYDOWN, VK_TAB, 0);

	//Init options
	init_ALL_OPTIONS(hwnd);

	//init Wwise Connection
	handleUI_B_Connect();

	return TRUE;
}
//=============================================================================
inline int CreateImportWindow::ErrMsg(const std::string& s)
{
	return MessageBox(0, s.c_str(), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
}

//////////////////////////////////
////	Handle UI notifications 


void CreateImportWindow::handleUI_B_Connect()
{
	if (parentWwiseConnectionHnd->handle_GUI_Connect())
	{
		SetStatusMessageText("Ready");
		SetDlgItemText(m_hWindow, IDC_WwiseConnection, "Wwise Connection Established");
		//		SendMessage(hwnd_combo, CB_SETCURSEL, 0, 0);
		//		textConnectionStatus
	}
	else
	{
		SetStatusMessageText("Error");
		SetDlgItemText(m_hWindow, IDC_WwiseConnection, "!!Wwise Connection Missing!!");
	}
}

void CreateImportWindow::handleUI_B_CreateObject()
{
	PrintToConsole("Creating New Wwise Object");
	CreateObjectArgs myCreateObjectArgs;

	handleUI_GetType(1);
	handleUI_GetNameConflict(1);

	///Get the par ID text
	char buffer[256];

	GetDlgItemTextA(m_hWindow,IDC_ImportParent_ID, buffer, 256);
	std::string s_parID = buffer;
	myCreateObjectArgs.ParentID = s_parID;

	GetDlgItemTextA(m_hWindow, IDC_text_CreateName, buffer, 256);
	std::string s_name = buffer;
	myCreateObjectArgs.Name = s_name;

	GetDlgItemTextA(m_hWindow, IDC_Text_CreateNotes, buffer, 256);
	std::string s_notes = buffer;
	myCreateObjectArgs.Notes = s_notes;

	myCreateObjectArgs.Type = s_CreateType;
	myCreateObjectArgs.onNameConflict = s_CreateNameConflict;




	AK::WwiseAuthoringAPI::AkJson::Array results;
	if (!parentWwiseConnectionHnd->CreateWwiseObjects(false, myCreateObjectArgs, results))
	{
		//ERROR
		SetStatusMessageText("Error");
		return;
	}
	SetStatusMessageText("Ready");

	//waapi_CreateObjectFromArgs(myCreateObjectArgs, results);
}

void CreateImportWindow::handleUI_B_GetSelectedParent()
{
	WwiseObject selectedParent = parentWwiseConnectionHnd->GetSelectedObject();
	std::string parID = selectedParent.properties["id"];
	std::string parNameType = selectedParent.properties["name"] + " (" + selectedParent.properties["type"] + ")";
	SetDlgItemText(m_hWindow, IDC_ImportParent_ID, parID.c_str());
	SetDlgItemText(m_hWindow, IDC_ImportParent_NameType, parNameType.c_str());

	HandleUI_SetParentForRenderJob(selectedParent);

}

void CreateImportWindow::handleUI_GetType(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(tr_c_CreateType, CB_GETCURSEL, 0, 0);
		s_CreateType = myCreateChoices.waapiCREATEchoices_TYPE[x];
		break;
	default:
		break;
	}
}

void CreateImportWindow::handleUI_GetNameConflict(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(tr_c_CreateNameConflict, CB_GETCURSEL, 0, 0);
		s_CreateNameConflict = myCreateChoices.waapiCREATEchoices_NAMECONFLICT[x];
		break;
	default:
		break;
	}
}

bool CreateImportWindow::UpdateProgressDuringRender(int numJobs)
{
	AllDone = false;
	numOfRendersDone = 0;
	
	while (!AllDone)
	{
		
		if (numOfRendersDone == numJobs) {
			AllDone = true;
			//PrintToConsole("ALL Renders complete");
			return true;
		}

		int jobIndex = 0;
		for (auto job : GlobalListOfRenderQueJobs)
		{
			if (!GlobalListOfRenderQueJobs[jobIndex].hasRendered)
			{
				WIN32_FIND_DATA FindFileData;
				HANDLE hFind = FindFirstFile(job.RenderQueFilePath.c_str(), &FindFileData);

				if (hFind == INVALID_HANDLE_VALUE)
				{
					if (GetLastError() == ERROR_FILE_NOT_FOUND)
					{
						if (!GlobalListOfRenderQueJobs[jobIndex].hasRendered)
						{
							PostMessage(tr_Progress_Import, PBM_STEPIT, 0, 0);

							GlobalListOfRenderQueJobs[jobIndex].hasRendered = true;
							numOfRendersDone++;
						}
					}
				}
				jobIndex++;
			}
				
		}

	}
	
	
}



void CreateImportWindow::handleUI_RenderImport()
{
	/// Render and import from que list

	//For each job in the global render que
	//check it has valid wwise parent and report errors/exit if not

	for (auto job : GlobalListOfRenderQueJobs)
	{
		if (job.parentWwiseObject.properties.size() == 0)
		{
			PrintToConsole("  ERROR! A render job has no valid Wwise Parent!   \n");
			PrintToConsole(job.RenderQueFilePath);
			SetStatusMessageText("Error");
			return;
		}

	}

	SetStatusMessageText("Rendering from Reaper Render Que");

	int numJobs = GlobalListOfRenderQueJobs.size();
	bool startedRender = false;

	SendMessage(tr_Progress_Import, PBM_SETRANGE, 0, MAKELPARAM(0, numJobs));

	SendMessage(tr_Progress_Import, PBM_SETSTEP, (WPARAM)1, 0);

	std::future<bool> fut = std::async(std::launch::async,&CreateImportWindow::UpdateProgressDuringRender,this,numJobs);

	ReaperRenderObj renderObj;
	renderObj.RenderAllQues();

	PrintToConsole("Waiting for second thread");

	fut.wait();
	bool ret = fut.get();

	if (ret)
	{
		PrintToConsole("Rejoined main");
		ImportJobsIntoWwise();
	}

}



bool CreateImportWindow::ImportJobsIntoWwise()
{
	SetStatusMessageText("Importing into Wwise");
	return false;
}



/// INIT ALL OPTIONS

bool CreateImportWindow::init_ALL_OPTIONS(HWND hwnd)
{
	tr_buttonConnect = GetDlgItem(hwnd, IDC_B_ConnectWwise);
	tr_textConnectionStatus = GetDlgItem(hwnd, IDC_WwiseConnection);
	tr_B_GetSelectedParent = GetDlgItem(hwnd, IDC_B_GetSelectedParent);
	tr_s_ImportParentID = GetDlgItem(hwnd, IDC_ImportParent_ID);
	tr_s_ImportParentNameType = GetDlgItem(hwnd, IDC_ImportParent_NameType);
	tr_B_CreateObject = GetDlgItem(hwnd, IDC_B_CreateObject);
	tr_c_CreateType = GetDlgItem(hwnd, IDC_C_Create_Type);
	tr_c_CreateNameConflict = GetDlgItem(hwnd, IDC_C_CreateOnNameConflict);
	tr_txt_CreateName = GetDlgItem(hwnd, IDC_text_CreateName);
	tr_txt_CreateNotes = GetDlgItem(hwnd, IDC_Text_CreateNotes);
	tr_Tree_RenderJobTree = GetDlgItem(hwnd, IDC_TREE_RenderJobTree);
	tr_Progress_Import = GetDlgItem(hwnd, IDC_PROGRESS_Import);
	B_RenderImport = GetDlgItem(hwnd, IDC_B_RenderImport);
	txt_status = GetDlgItem(hwnd, IDC_Txt_Status);
	txt_Language = GetDlgItem(hwnd, IDC_Language);
	Edit_SetText(txt_Language, defaultLanguage.c_str());
	check_IsVoice = GetDlgItem(hwnd, IDC_IsVoice);
	check_OrigDirMatchWwise = GetDlgItem(hwnd, IDC_OrigsMatchWwise);
	SendDlgItemMessage(m_hWindow, IDC_OrigsMatchWwise, BM_SETCHECK, BST_CHECKED, 0);

	init_ComboBox_A(tr_c_CreateType, myCreateChoices.waapiCREATEchoices_TYPE);
	init_ComboBox_A(tr_c_CreateNameConflict, myCreateChoices.waapiCREATEchoices_NAMECONFLICT);

	FillRenderQueList(hwnd);

	SetStatusMessageText("Ready");

	return true;
}

///Initialise dialogue boxes
bool CreateImportWindow::init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice : choices)
	{
		SendMessage(hwnd_combo, CB_ADDSTRING, i, (LPARAM)choice.c_str());
		SendMessage(hwnd_combo, CB_SETCURSEL, 0, 0);
		i++;
	}
	return true;
}

bool CreateImportWindow::init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice : choices)
	{
		SendMessage(hwnd_list, LB_ADDSTRING, i, (LPARAM)choice.c_str());
		i++;
	}
	return true;
}


void CreateImportWindow::FillRenderQueList(HWND hwnd)	
{
	GlobalListOfRenderQueJobs.clear();
	std::vector<std::string> ListOfRenderQueFiles;
	ListOfRenderQueFiles = GetListOfRenderQues();

	for (auto RenderQueFile : ListOfRenderQueFiles)
	{
		RenderQueJob MyrenderQueJob = CreateRenderQueJobFromRenderQueFile(RenderQueFile);
		if (!MyrenderQueJob.RenderQueJobFileList.empty())
		{
			GlobalListOfRenderQueJobs.push_back(MyrenderQueJob);
		}
	}

	UpdateRenderJob_TreeView(hwnd);

}

void CreateImportWindow::UpdateRenderJob_TreeView(HWND hwnd)
{
	for (auto RenderJob : GlobalListOfRenderQueJobs)
	{

		TV_INSERTSTRUCT tvInsert;
		HTREEITEM Parent;
		HTREEITEM Child;

		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = TVI_ROOT;
		tvInsert.item.mask = TVIF_TEXT;	// tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tvInsert.item.pszText = &RenderJob.RenderQueFilePath[0];	//(LPARAM)choice.c_str()
		Parent = (HTREEITEM)SendDlgItemMessage(m_hWindow, IDC_TREE_RenderJobTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);

		//Children = WAV files in this render que job
		for (auto renderFile : RenderJob.RenderQueJobFileList)
		{
			tvInsert.hParent = Parent;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.pszText = &renderFile[0];
			Child = (HTREEITEM)SendDlgItemMessage(m_hWindow, IDC_TREE_RenderJobTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);
		}


	}

}


void CreateImportWindow::HandleUI_SetParentForRenderJob(WwiseObject selectedParent)
{
	std::string parentWwiseID = selectedParent.properties["id"];
	std::string parentWwiseName = selectedParent.properties["name"];
	std::string parentWwiseType = selectedParent.properties["type"];
	TVITEM item;

	if (parentWwiseID == "") return;	// Invalid wwise parent selection!

	HTREEITEM hSelectedItem = TreeView_GetSelection(tr_Tree_RenderJobTree);
	if (hSelectedItem == NULL) // Nothing selected
	{
		return;
	}
	TCHAR buffer[256];
	item.hItem = hSelectedItem;
	item.mask = TVIF_TEXT | TVIF_CHILDREN;
	item.cchTextMax = 256;
	item.pszText = buffer;
	if (TreeView_GetItem(tr_Tree_RenderJobTree, &item))
	{
		if (item.cChildren != 1)
		{
			PrintToConsole("Render Job selected has no children");
			return;
		}
		//Find the matching Render Que Job
		std::string itemName = item.pszText;
		int count = 0;
		for (auto renderJob : GlobalListOfRenderQueJobs)
		{
			if (itemName.find(renderJob.RenderQueFilePath) != itemName.npos)
			{
				//Found a match
				//PrintToConsole("Found a match");
				GlobalListOfRenderQueJobs[count].parentWwiseObject = selectedParent;

				GlobalListOfRenderQueJobs[count].isVoice = GetIsVoice();

				GlobalListOfRenderQueJobs[count].OrigDirMatchesWwise = GetOrigsDirMatchesWwise();

				std::string language;
				if (GlobalListOfRenderQueJobs[count].isVoice)
				{
					GlobalListOfRenderQueJobs[count].ImportLanguage = GetLanguage();
					language = GlobalListOfRenderQueJobs[count].ImportLanguage;
				}
				else
				{
					GlobalListOfRenderQueJobs[count].ImportLanguage = "SFX";
					language = "SFX";
				}

				//Set the display Text to include wwise parent name and type
				std::string newItemName = parentWwiseName + "(" + parentWwiseType +" : "+ language + ")  - " + renderJob.RenderQueFilePath;
				item.mask = TVIF_TEXT;
				item.pszText = &newItemName[0];
				TreeView_SetItem(tr_Tree_RenderJobTree, &item);
				PrintToConsole(renderJob.RenderQueFilePath + " Imports into " + renderJob.parentWwiseObject.properties["name"]);
			}
			count++;
		}

	}

	SetStatusMessageText("Ready");

	//for (auto renderJob : GlobalListOfRenderQueJobs)
	//{
	//	PrintToConsole(renderJob.RenderQueFilePath + " Imports into " + renderJob.parentWwiseObject.properties["name"]);
	//}


}

bool CreateImportWindow::GetIsVoice()
{

	if (SendDlgItemMessage(m_hWindow, IDC_IsVoice, BM_GETCHECK, 0, 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::string CreateImportWindow::GetLanguage()
{
	///Get the par ID text
	char buffer[256];
	GetDlgItemTextA(m_hWindow, IDC_Language, buffer, 256);
	std::string lang = buffer;
	return lang;
}

bool CreateImportWindow::GetOrigsDirMatchesWwise()
{
	if (SendDlgItemMessage(m_hWindow, IDC_OrigsMatchWwise, BM_GETCHECK, 0, 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CreateImportWindow::SetStatusMessageText(std::string message)
{
	SetDlgItemText(m_hWindow, IDC_Txt_Status, message.c_str());
}




