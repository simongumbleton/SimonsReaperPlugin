#pragma once
#include <Windows.h>
#include <Commctrl.h>
#include <string>

#include "reaper_plugin.h"

//globals
extern HWND g_parentWindow;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

void GetReaperGlobals();

bool HookCommandProc(int command, int flag);

void doAction1();

void PrintToConsole(std::string text);
