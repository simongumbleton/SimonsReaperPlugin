#include "PluginWindow.h"
#include "resource.h"



//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnCommand(const HWND, int, int, const HWND);
INT_PTR OnInitDlg(const HWND, LPARAM);


///Handles to UI elements
HWND comboBoxFROM;
HWND textBoxFROM_Uinput;
HWND comboBoxSELECT;
HWND comboBoxWHERE;
HWND textBoxWHERE_Uinput;
HWND listBoxRETURN;
HWND buttonGO;

GetObjectChoices myGetObjectChoices;



//non-message function declarations
inline int ErrMsg(const ustring&);
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	INT_PTR success = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);

	if (success == -1)
	{
		ErrMsg(_T("DialogBox failed."));
	}
	return 0;
}
//=============================================================================
INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
//=============================================================================
void OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl)
{
	//handles WM_COMMAND message of the modal dialogbox
	switch (id)
	{
	case ID_B_OK:        //RETURN key pressed or 'ok' button selected
		MessageBox(NULL, "Ok buttor", _T("DEBUG"), MB_OK | MB_ICONEXCLAMATION);
		break;
	case IDC_COMBO_GetFrom:
		handleUI_GetFrom(notifycode);
		break;
	case ID_B_CANCEL:    //ESC key pressed or 'cancel' button selected
		EndDialog(hwnd, id);
	}
}
//=============================================================================
INT_PTR OnInitDlg(const HWND hwnd, LPARAM lParam)
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


	return TRUE;
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
	return MessageBox(0, s.c_str(), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
}

//////////////////////////////////
////	Handle UI notifications 
void handleUI_GetFrom(int notifCode)
{
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		break;
	default:
		break;
	}
}







/// INIT ALL OPTIONS

bool init_ALL_OPTIONS(HWND hwnd)
{
	comboBoxFROM = GetDlgItem(hwnd, IDC_COMBO_GetFrom);
	textBoxFROM_Uinput = GetDlgItem(hwnd, IDC_GetFrom_Uinput);
	comboBoxSELECT = GetDlgItem(hwnd, IDC_COMBO_GetSelect);
	comboBoxWHERE = GetDlgItem(hwnd, IDC_COMBO_GetWhere);
	textBoxWHERE_Uinput = GetDlgItem(hwnd, IDC_GetWhere_Uinput);
	listBoxRETURN = GetDlgItem(hwnd, IDC_LIST_ReturnOptions);
	buttonGO = GetDlgItem(hwnd, ID_B_GO);

	
	init_ComboBox_A(comboBoxFROM, myGetObjectChoices.waapiGETchoices_FROM);
	init_ComboBox_A(comboBoxSELECT, myGetObjectChoices.waapiGETchoices_SELECT);
	init_ComboBox_A(comboBoxWHERE, myGetObjectChoices.waapiGETchoices_WHERE);
	init_ListBox_A(listBoxRETURN,myGetObjectChoices.waapiGETchoices_RETURN);
	return true;
}

///Initialise dialogue boxes
bool init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices)
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

bool init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice : choices)
	{
		SendMessage(hwnd_list, LB_ADDSTRING, i, (LPARAM)choice.c_str());
		i++;
	}
	return true;
}
