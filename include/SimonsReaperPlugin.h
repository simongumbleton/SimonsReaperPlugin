#pragma once
#include <Windows.h>

#include "reaper_plugin.h"

//globals
extern HWND g_parentWindow;
extern REAPER_PLUGIN_HINSTANCE g_hInst;
char currentProject[256];
char reaperProjectName[256];

void GetReaperGlobals();

bool HookCommandProc(int command, int flag);

void doAction1();

void doAction2();