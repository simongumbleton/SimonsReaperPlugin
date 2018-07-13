#include <windows.h>
#include <Wincodec.h>
#include <Commctrl.h>
#include "stdio.h"
//#include "stdlib.h"
//#include <string>
#include <iostream> 
#include <sstream>

#include "waapiconnect.h"
#include <AK/WwiseAuthoringAPI/AkVariantBase.h>
#include <AkAutobahn/AkJson.h>
#include <AK/WwiseAuthoringAPI/waapi.h>



//WAAPIConnect::WAAPIConnect(HWND window, int treeId, int statusTextid, int transferWindowId)

	//: hwnd(window)
	//	, m_wwiseViewId(treeId)
	//	, m_statusTextId(statusTextid)
	//	, m_transferWindowId(transferWindowId)
	//	, m_progressWindow(0)
	//	, m_client()
WAAPIConnect::WAAPIConnect()	/// This is the constructor, gets called with every instance of WAAPIConnect
	{
		//Connect();	/// Connect doesn't need to be called in the constructor
	}


bool WAAPIConnect::Connect(bool suppressOuputMessages)	//Connect to Wwise. Optionally pass bool true to supress message boxes from wwise connection
{
	using namespace AK::WwiseAuthoringAPI;
	AkJson wwiseInfo;
	bool success = false;

	if (success = m_client.Connect("127.0.0.1", g_Waapi_Port))
	{
		//Get Wwise info
		if (success = m_client.Call(ak::wwise::core::getInfo,
			AkJson(AkJson::Type::Map),
			AkJson(AkJson::Type::Map),
			wwiseInfo))
		{

			//create a status text string and set it
			std::stringstream status;
			status << "Connected on port " + std::to_string(g_Waapi_Port) + ": ";
			status << wwiseInfo["displayName"].GetVariant().GetString();
			status << " - " + wwiseInfo["version"]["displayName"].GetVariant().GetString();
			std::string WwiseConnectionStatus = status.str();
			if (!suppressOuputMessages)
			{
				MessageBox(NULL, WwiseConnectionStatus.c_str(), "Wwise Connection Status", MB_OK);
			}
		}
	}

	if (!success)
	{
		if (!suppressOuputMessages)
		{
			MessageBox(NULL, "! Failed to Connect !", "Wwise Connection Status", MB_OK);
		}
	}

	return success;
}
