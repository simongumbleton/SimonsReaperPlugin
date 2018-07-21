#include "PluginWindow.h"
#include "resource.h"



//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnCommand(const HWND, int, int, const HWND);
INT_PTR OnInitDlg(const HWND, LPARAM);

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


/// INIT ALL OPTIONS

bool init_ALL_OPTIONS(HWND hwnd)
{
	HWND comboBox = GetDlgItem(hwnd, IDC_COMBO1);
	GetObjectChoices myGetObjectChoices;
	init_ComboBox_A(comboBox, myGetObjectChoices.waapiGETchoices_FROM);
	return true;
}

///Initialise dialogue boxes
bool init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice: choices)
	{
		SendMessage(hwnd_combo, CB_ADDSTRING, i, (LPARAM)choice.c_str());
		i++;
	}
	return true;
}