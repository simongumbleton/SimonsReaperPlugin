#pragma once
#include "WwiseConnectionHandler.h"
#include "resource.h"
#include "PluginWindow.h"
#include "SimonsReaperPlugin.h"

///Handles to UI elements
HWND comboBoxFROM;
std::string s_comboBoxFROM;
HWND textBoxFROM_Uinput;
std::string s_textBoxFROM_Uinput;
HWND comboBoxSELECT;
std::string s_comboBoxSELECT;
HWND comboBoxWHERE;
std::string s_comboBoxWHERE;
HWND textBoxWHERE_Uinput;
std::string s_textBoxWHERE_Uinput;
HWND listBoxRETURN;
int i_listBoxRETURN[16] = {};
std::vector<std::string> s_listBoxRETURN{};
HWND buttonGO;
HWND buttonConnect;
HWND textConnectionStatus;
HWND buttonTREE;
HWND wwiseTree;

GetObjectChoices myGetObjectChoices;

ObjectGetArgs getArgsFromGUI;

HWND PluginWindow::m_hWindow = NULL;
long PluginWindow::m_lSaveThis = 0;
WwiseConnectionHandler* PluginWindow::parentWwiseConnectionHnd = NULL;


//=============================================================================
int PluginWindow::CreatePluginWindow(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	INT_PTR success = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProcStatic);

	if (success == -1)
	{
		ErrMsg(_T("DialogBox failed."));
	}
	return 0;
}

void PluginWindow::SetupPluginParent(WwiseConnectionHandler * parent)
{
	parentWwiseConnectionHnd = parent;
	thisPluginWindow = this;
}

INT_PTR CALLBACK PluginWindow::DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

INT_PTR PluginWindow::DialogProcStatic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWindow = hDlg;

	PluginWindow *pThis = (PluginWindow*)m_lSaveThis;

	return (pThis->DlgProc(hDlg, message, wParam, lParam));
}


PluginWindow::PluginWindow()
{
	m_lSaveThis = (long)this;
	saveThis = this;
}

PluginWindow::~PluginWindow()
{
}

int PluginWindow::DoModal(void)
{
	return 0;
}




//=============================================================================
void PluginWindow::OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl)
{
	//handles WM_COMMAND message of the modal dialogbox
	switch (id)
	{
	case ID_B_GO:        //RETURN key pressed or 'GO' button selected
		//MessageBox(NULL, "Ok buttor", _T("DEBUG"), MB_OK | MB_ICONEXCLAMATION);
		handleUI_B_GO();
		break;
	case IDC_COMBO_GetFrom:
		handleUI_GetFrom(notifycode);
		break;
	case IDC_B_ConnectWwise:
		handleUI_B_Connect();
		break;
	case IDC_COMBO_GetSelect:
		handleUI_GetSelect(notifycode);
		break;
	case IDC_COMBO_GetWhere:
		handleUI_GetWhere(notifycode);
		break;
	case IDC_LIST_ReturnOptions:
		handleUI_GetReturnOptions(notifycode);
		break;
	case ID_B_CANCEL:    //ESC key pressed or 'cancel' button selected
		EndDialog(hwnd, id);
	}
}
//=============================================================================
INT_PTR PluginWindow::OnInitDlg(const HWND hwnd, LPARAM lParam)
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
inline int PluginWindow::ErrMsg(const std::string& s)
{
	return MessageBox(0, s.c_str(), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
}

//////////////////////////////////
////	Handle UI notifications 
void PluginWindow::handleUI_GetFrom(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(comboBoxFROM, CB_GETCURSEL, 0, 0);
		if (x != 0)	// Somethig other than Wwise Selected
		{
			Edit_Enable(textBoxFROM_Uinput,true);
		}
		else
		{
			Edit_Enable(textBoxFROM_Uinput, false);
		}
		s_comboBoxFROM = myGetObjectChoices.waapiGETchoices_FROM[x];
		break;
	default:
		break;
	}
}

void PluginWindow::handleUI_GetSelect(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(comboBoxSELECT, CB_GETCURSEL, 0, 0);
		s_comboBoxSELECT = myGetObjectChoices.waapiGETchoices_SELECT[x];
		break;
	default:
		break;
	}
}

void PluginWindow::handleUI_GetWhere(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(comboBoxWHERE, CB_GETCURSEL, 0, 0);
		s_comboBoxWHERE = myGetObjectChoices.waapiGETchoices_WHERE[x];
		break;
	default:
		break;
	}
}

void PluginWindow::handleUI_GetReturnOptions(int notifCode)
{		
	
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		std::fill_n(i_listBoxRETURN, 16, -1);
		SendMessage(listBoxRETURN, LB_GETSELITEMS, 256,(LPARAM)&i_listBoxRETURN);
		break;
	default:
		break;
	}
}

void PluginWindow::handleUI_B_Connect()
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

void PluginWindow::handleUI_B_GO()
{
	/// GO pressed. Fill in the required structures for Object Get call from UI elements
	PrintToConsole("Recreating Wwise Tree with results from core::object::Get");

	///Clear the vecotrs for data storage
	getArgsFromGUI.From.clear();
	getArgsFromGUI.Where.clear();
	getArgsFromGUI.Select = "";
	getArgsFromGUI.customReturnArgs.clear();
	s_listBoxRETURN.clear();

	///Force a "Selection change call on the drop downs" in case use made not selections away from the default
	handleUI_GetFrom(1);
	handleUI_GetSelect(1);
	handleUI_GetWhere(1);
	handleUI_GetReturnOptions(1);
	///Get the FROM text if not selected
	char buffer[256];
	Edit_GetText(textBoxFROM_Uinput, buffer, 256);
	s_textBoxFROM_Uinput = buffer;

	///Get the where text
	Edit_GetText(textBoxWHERE_Uinput, buffer, 256);
	s_textBoxWHERE_Uinput = buffer;
	//s_textBoxWHERE_Uinput

	///fill in the return options array
	///s_listBoxRETURN  from i_listBoxRETURN indexes
	for (auto index : i_listBoxRETURN)
	{
		if (index != -1)
		{
			s_listBoxRETURN.push_back(myGetObjectChoices.waapiGETchoices_RETURN[index]);
		}
	}
	///Fill in the args from GUI - getArgsFromGUI

	getArgsFromGUI.fromSelected = (s_comboBoxFROM == "Wwise Selection"); // if choice is Wwise Selection
	getArgsFromGUI.From = { s_comboBoxFROM ,s_textBoxFROM_Uinput };
	getArgsFromGUI.Select = s_comboBoxSELECT;
	getArgsFromGUI.Where = {s_comboBoxWHERE,s_textBoxWHERE_Uinput};
	getArgsFromGUI.customReturnArgs = s_listBoxRETURN;
	PrintToConsole("Args collected");

	AK::WwiseAuthoringAPI::AkJson::Array results;
	std::vector<WwiseObject> MyWwiseObjects;
	try {
		MyWwiseObjects = parentWwiseConnectionHnd->GetWwiseObjects(false, getArgsFromGUI, results);
	}catch (std::string e) {
		PrintToConsole(e);
	}

	for (const auto obj : MyWwiseObjects) {
		PrintToConsole("");
		PrintToConsole(obj.properties.at("name"));
		for (auto prpty : obj.properties) {
			PrintToConsole(prpty.first + " = " + prpty.second);
			
		}
	}

}

void PluginWindow::handleUI_B_WwiseTree()
{

}

/// INIT ALL OPTIONS

bool PluginWindow::init_ALL_OPTIONS(HWND hwnd)
{
	comboBoxFROM = GetDlgItem(hwnd, IDC_COMBO_GetFrom);
	textBoxFROM_Uinput = GetDlgItem(hwnd, IDC_GetFrom_Uinput);
	comboBoxSELECT = GetDlgItem(hwnd, IDC_COMBO_GetSelect);
	comboBoxWHERE = GetDlgItem(hwnd, IDC_COMBO_GetWhere);
	textBoxWHERE_Uinput = GetDlgItem(hwnd, IDC_GetWhere_Uinput);
	listBoxRETURN = GetDlgItem(hwnd, IDC_LIST_ReturnOptions);
	buttonGO = GetDlgItem(hwnd, ID_B_GO);
	buttonConnect = GetDlgItem(hwnd, IDC_B_ConnectWwise);
	textConnectionStatus = GetDlgItem(hwnd, IDC_WwiseConnection);
	buttonTREE = GetDlgItem(hwnd, IDC_B_Tree);
	wwiseTree = GetDlgItem(hwnd, IDC_TREE1);
		
	
	init_ComboBox_A(comboBoxFROM, myGetObjectChoices.waapiGETchoices_FROM);
	init_ComboBox_A(comboBoxSELECT, myGetObjectChoices.waapiGETchoices_SELECT);
	init_ComboBox_A(comboBoxWHERE, myGetObjectChoices.waapiGETchoices_WHERE);
	init_ListBox_A(listBoxRETURN,myGetObjectChoices.waapiGETchoices_RETURN);
	return true;
}

///Initialise dialogue boxes
bool PluginWindow::init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice: choices)
	{
		SendMessage(hwnd_combo, CB_ADDSTRING, i, (LPARAM)choice.c_str());
		SendMessage(hwnd_combo, CB_SETCURSEL, 0, 0);
		i++;
	}
	return true;
}

bool PluginWindow::init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice : choices)
	{
		SendMessage(hwnd_list, LB_ADDSTRING, i, (LPARAM)choice.c_str());
		i++;
	}
	return true;
}
