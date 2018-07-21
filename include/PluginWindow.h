#pragma once
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>    //string and other mapping macros
#include <string>
#include "WaapiFunctions.h"


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
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int);
//=============================================================================
INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//=============================================================================
void OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl);
//=============================================================================
INT_PTR OnInitDlg(const HWND hwnd, LPARAM lParam);
//=============================================================================
inline int ErrMsg(const ustring& s);


///Handle the various UI elements
void handleUI_GetFrom(int notifCode);


///Initialise dialogue boxes

bool init_ALL_OPTIONS(HWND hwnd);

bool init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices);
bool init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices);
