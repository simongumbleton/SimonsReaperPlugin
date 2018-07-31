#include <Wincodec.h>


#include "stdio.h"
#include "stdlib.h"

#include <iostream> 
#include <sstream>

#include "reaper_plugin.h"

#define REAPERAPI_IMPLEMENT
#include "reaper_plugin_functions.h"
#include <AkAutobahn\AkJson.h>

#include "SimonsReaperPlugin.h"

#include "WwiseConnectionHandler.h"



#define GET_FUNC_AND_CHKERROR(x) if (!((*((void **)&(x)) = (void *)rec->GetFunc(#x)))) ++funcerrcnt
#define REGISTER_AND_CHKERROR(variable, name, info) if(!(variable = rec->Register(name, (void*)info))) ++regerrcnt

//define globals
HWND g_parentWindow;
HINSTANCE g_hInst;
char reaperProjectName[256];
std::string reaperResourcePath;
int WaapiPort = 8095;

char currentProject[256];
bool supressMessagebox = false;
bool supressConsoleOutput = false;


WwiseConnectionHandler MyWwiseConnectionHandler;


//actions
gaccel_register_t action01 = { { 0, 0, 0 }, "Do action 01." };
gaccel_register_t connectToWwise = { { 0, 0, 0 }, "Do action 02." };
gaccel_register_t Transfer_To_Wwise = { { 0, 0, 0 }, "Do action 03." };

//produces an error message during reaper startup
//similar to SWS function ErrMsg in sws_extension.cpp
void StartupError(const std::string &errMsg, 
                  uint32_t messageBoxFlags = MB_ICONERROR, 
                  const std::string &caption = "Error")
{
    if (!IsREAPER || IsREAPER())
    {
        HWND msgBoxHwnd = Splash_GetWnd ? Splash_GetWnd() : nullptr;
        MessageBox(msgBoxHwnd, errMsg.c_str(), caption.c_str(), MB_OK | messageBoxFlags);
    }
}


extern "C"
{
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t *rec)
    {

        //return if plugin is exiting
        if (!rec)
        {
            return 0;
        }
        //set globals
        g_parentWindow = rec->hwnd_main;
        g_hInst = hInstance;

        //get func pointers that we need
        int funcerrcnt = 0;
        GET_FUNC_AND_CHKERROR(Main_OnCommand);
        GET_FUNC_AND_CHKERROR(EnumProjectMarkers2);
        GET_FUNC_AND_CHKERROR(ShowConsoleMsg);
        GET_FUNC_AND_CHKERROR(GetMainHwnd);
        GET_FUNC_AND_CHKERROR(GetResourcePath);
        GET_FUNC_AND_CHKERROR(GetProjectPath);
        GET_FUNC_AND_CHKERROR(EnumProjects);
        GET_FUNC_AND_CHKERROR(Main_openProject);
        GET_FUNC_AND_CHKERROR(GetProjectName);
        GET_FUNC_AND_CHKERROR(AddExtensionsMainMenu);
        GET_FUNC_AND_CHKERROR(plugin_register);
        GET_FUNC_AND_CHKERROR(IsREAPER);
        GET_FUNC_AND_CHKERROR(Splash_GetWnd);
        GET_FUNC_AND_CHKERROR(EnumProjectMarkers);
        GET_FUNC_AND_CHKERROR(GetTrack);
        GET_FUNC_AND_CHKERROR(GetTrackGUID);
        GET_FUNC_AND_CHKERROR(guidToString);
        GET_FUNC_AND_CHKERROR(GetSetMediaTrackInfo_String);
        GET_FUNC_AND_CHKERROR(CountTracks);
        GET_FUNC_AND_CHKERROR(CountProjectMarkers);

        //exit if any func pointer couldn't be found
        if (funcerrcnt)
        {
            StartupError("An error occured whilst initializing.\n"
                         "Try updating to the latest Reaper version.");
            return 0;
        }

        //register commands
        int regerrcnt = 0;
        REGISTER_AND_CHKERROR(action01.accel.cmd, "command_id", "action01");
        REGISTER_AND_CHKERROR(connectToWwise.accel.cmd, "command_id", "action02");
		REGISTER_AND_CHKERROR(Transfer_To_Wwise.accel.cmd, "command_id", "action03");
        if (regerrcnt)
        {
            StartupError("An error occured whilst initializing.\n"
                         "Try updating to the latest Reaper version.");
            return 0;
        }

        //register actions
        plugin_register("gaccel", &connectToWwise.accel);
        plugin_register("gaccel", &action01.accel);
		plugin_register("gaccel", &Transfer_To_Wwise.accel);

        rec->Register("hookcommand", (void*)HookCommandProc);

        AddExtensionsMainMenu();

        HMENU hMenu = GetSubMenu(GetMenu(GetMainHwnd()), 8);
        {
            MENUITEMINFO mi = { sizeof(MENUITEMINFO), };
            mi.fMask = MIIM_TYPE | MIIM_ID;
            mi.fType = MFT_STRING;
            mi.wID = action01.accel.cmd;
            mi.dwTypeData = "Action 01";
            InsertMenuItem(hMenu, 0, true, &mi);
        }
        {
            MENUITEMINFO mi = { sizeof(MENUITEMINFO), };
            mi.fMask = MIIM_TYPE | MIIM_ID;
            mi.fType = MFT_STRING;
            mi.wID = connectToWwise.accel.cmd;
            mi.dwTypeData = "Connect To Wwise";
            InsertMenuItem(hMenu, 1, true, &mi);
        }
		{
			MENUITEMINFO mi = { sizeof(MENUITEMINFO), };
			mi.fMask = MIIM_TYPE | MIIM_ID;
			mi.fType = MFT_STRING;
			mi.wID = Transfer_To_Wwise.accel.cmd;
			mi.dwTypeData = "Transfer To Wwise";
			InsertMenuItem(hMenu, 2, true, &mi);
		}
		
    }
}


bool HookCommandProc(int command, int flag)
{
	MyWwiseConnectionHandler.MyCurrentWwiseConnection.port = WaapiPort;
	MyWwiseConnectionHandler.MyCurrentWwiseConnection.supressDebugOutput = supressMessagebox;
	GetReaperGlobals();
    if (command == action01.accel.cmd)
    {
		doAction1();
        return true;
    }
    if (command == connectToWwise.accel.cmd)
    {
		if (!MyWwiseConnectionHandler.StartGUI_Get(g_hInst))
		{
			return false;
		}
		//MyWwiseConnectionHandler.ConnectToWwise(supressMessagebox, WaapiPort);
		return true;
    }
	if (command == Transfer_To_Wwise.accel.cmd)
	{
		if (!MyWwiseConnectionHandler.StartGUI_Transfer(g_hInst))
		{
			return false;
		}
		//MyWwiseConnectionHandler.GetChildrenFromSelectedParent(supressMessagebox);
		return true;
	}
    return false;
}

void doAction1()
{
	GetReaperGlobals();
	
	ShowConsoleMsg(reaperProjectName);
	MessageBox(g_parentWindow, "Hello World!", "Reaper extension API test", MB_OK);
}

void PrintToConsole(std::string text)
{
	if (!supressConsoleOutput)
	{
		std::string debugText = text + "\n";
		ShowConsoleMsg(debugText.c_str());
	}
}

std::string GetReaperResourcePath()
{
	return GetResourcePath();
}

std::string GetCurrentReaperProject()
{
	char projName[256];
	EnumProjects(-1, currentProject, MAX_PATH);
	GetProjectName(EnumProjects(-1, nullptr, 0), projName, 256);
	return std::string(projName);
}

void GetReaperGlobals()
{
	//get open project and compare
	EnumProjects(-1, currentProject, MAX_PATH);
	GetProjectName(EnumProjects(-1, nullptr, 0), reaperProjectName, 256);
	reaperResourcePath = GetResourcePath();
}


