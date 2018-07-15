#include <iostream> 
#include <sstream>
#include <windows.h>

#include "WaapiFunctions.h"
#include <AK/WwiseAuthoringAPI/AkVariantBase.h>
#include <AkAutobahn\AkJson.h>
#include <AK/WwiseAuthoringAPI/waapi.h>


///Socket client for Waapi connection
AK::WwiseAuthoringAPI::Client my_client;
int My_WAAPI_CLIENT_TIMEOUT_MS = 2000;
int my_Waapi_Port = 8095;

CurrentWwiseConnection waapi_CurrentWwiseConnection;

bool waapi_Connect(bool suppressOuputMessages)
{
	using namespace AK::WwiseAuthoringAPI;
	AkJson wwiseInfo;
	bool success = false;

	if (success = my_client.Connect("127.0.0.1", my_Waapi_Port))
	{
		//Get Wwise info
		if (success = my_client.Call(ak::wwise::core::getInfo,
			AkJson(AkJson::Type::Map),
			AkJson(AkJson::Type::Map),
			wwiseInfo))
		{
			waapi_CurrentWwiseConnection.port = my_Waapi_Port;
			waapi_CurrentWwiseConnection.Version = wwiseInfo["version"]["displayName"].GetVariant().GetString();

			if (!suppressOuputMessages)
			{ 
				//create a status text string and set it
				std::stringstream status;
				status << "Connected on port " + std::to_string(waapi_CurrentWwiseConnection.port) + ": ";
				status << " - " + waapi_CurrentWwiseConnection.Version;
				std::string WwiseConnectionStatus = status.str();
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


bool waapi_GetSelectedWwiseObjects(AK::WwiseAuthoringAPI::AkJson & resultsOut, bool getNotes)
{
	using namespace AK::WwiseAuthoringAPI;
	AkJson ReturnResults;
	AkJson options(AkJson::Map{
		{ "return", AkJson::Array{
			AkVariant("id"),
			AkVariant("name"),
			AkVariant("type"),
			AkVariant("path"),
			AkVariant("parent"),
			AkVariant("childrenCount"), } }
		});
	if (getNotes)
	{
		options["return"].GetArray().push_back(AkVariant("notes"));
	}
	return my_client.Call(ak::wwise::ui::getSelectedObjects, AkJson(AkJson::Map()), options, resultsOut);
}

void waapi_GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array & arrayIn, AK::WwiseAuthoringAPI::AkJson & results)
{
	using namespace AK::WwiseAuthoringAPI;
	switch (results.GetType())
	{
	case AkJson::Type::Map:
	{
		if (results.HasKey("objects"))
		{
			arrayIn = results["objects"].GetArray();
			return;
		}
		else if (results.HasKey("return"))
		{
			arrayIn = results["return"].GetArray();
			return;
		}
		else
		{
			assert(!"Not implemented.");
			return;
		}
	} break;
	default:
		assert(!"Not implemented.");
		return;
	}
}
