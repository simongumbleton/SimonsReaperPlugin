#include <iostream> 
#include <sstream>
#include <windows.h>

#include "WaapiFunctions.h"
#include <AK/WwiseAuthoringAPI/AkVariantBase.h>
#include <AkAutobahn\AkJson.h>
#include <AK/WwiseAuthoringAPI/waapi.h>


WaapiFunctions::WaapiFunctions()
{
}

bool WaapiFunctions::Connect(bool suppressOuputMessages)
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


bool WaapiFunctions::GetSelectedWwiseObjects(AK::WwiseAuthoringAPI::AkJson & resultsOut, bool getNotes)
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
	return m_client.Call(ak::wwise::ui::getSelectedObjects, AkJson(AkJson::Map()), options, resultsOut);
}

void WaapiFunctions::GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array & arrayIn, AK::WwiseAuthoringAPI::AkJson & results)
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
