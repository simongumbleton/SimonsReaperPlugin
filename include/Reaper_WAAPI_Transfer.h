#pragma once
#include <Windows.h>

#include "reaper_plugin.h"

//globals
extern HWND g_parentWindow;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

bool HookCommandProc(int command, int flag);