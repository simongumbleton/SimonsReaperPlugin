#pragma once
#include "WwiseConnectionHandler.h"
#include "resource.h"
#include "PluginWindow.h"
#include "SimonsReaperPlugin.h"

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

CreateObjectChoices myCreateChoices;

HWND CreateImportWindow::m_hWindow = NULL;
long CreateImportWindow::m_lSaveThis = 0;
WwiseConnectionHandler* CreateImportWindow::parentWwiseConnectionHnd = NULL;


//=============================================================================
int CreateImportWindow::CreateTransferWindow(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	INT_PTR success = DialogBox(hInst, MAKEINTRESOURCE(IDD_Transfer), 0, DialogProcStatic);

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
	if (m_hWindow == NULL)
	{
		m_hWindow = hDlg;
	}

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
	case ID_B_CANCEL:    //ESC key pressed or 'cancel' button selected
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
		SetDlgItemText(m_hWindow, IDC_WwiseConnection, "Wwise Connection Established");
		//		SendMessage(hwnd_combo, CB_SETCURSEL, 0, 0);
		//		textConnectionStatus
	}
	else
	{
		SetDlgItemText(m_hWindow, IDC_WwiseConnection, "!!Wwise Connection Missing!!");
	}
}

void CreateImportWindow::handleUI_B_CreateObject()
{
	PrintToConsole("Creating New Wwise Object");
	CreateObjectArgs myCreateObjectArgs;

	///Get the FROM text if not selected
	char buffer[256];
	GetDlgItemTextA(m_hWindow,IDC_ImportParent_ID, buffer, 256);
	std::string s_parID = buffer;
	myCreateObjectArgs.ParentID = s_parID;





	AK::WwiseAuthoringAPI::AkJson::Array results;
	parentWwiseConnectionHnd->CreateWwiseObjects(false,myCreateObjectArgs, results);

	//waapi_CreateObjectFromArgs(myCreateObjectArgs, results);
}

void CreateImportWindow::handleUI_B_GetSelectedParent()
{
	WwiseObject selectedParent = parentWwiseConnectionHnd->GetSelectedObject();
	std::string parID = selectedParent.properties["id"];
	std::string parNameType = selectedParent.properties["name"] + " (" + selectedParent.properties["type"] + ")";
	SetDlgItemText(m_hWindow, IDC_ImportParent_ID, parID.c_str());
	SetDlgItemText(m_hWindow, IDC_ImportParent_NameType, parNameType.c_str());

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

	init_ComboBox_A(tr_c_CreateType, myCreateChoices.waapiCREATEchoices_TYPE);
	init_ComboBox_A(tr_c_CreateNameConflict, myCreateChoices.waapiCREATEchoices_NAMECONFLICT);

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
