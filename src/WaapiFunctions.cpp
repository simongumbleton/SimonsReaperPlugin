#include <iostream> 
#include <sstream>

#include "WaapiFunctions.h"
#include <AK/WwiseAuthoringAPI/AkVariantBase.h>
#include <AkAutobahn\AkJson.h>
#include <AK/WwiseAuthoringAPI/waapi.h>
#include <AkAutobahn\JSONHelpers.h>
#include "SimonsReaperPlugin.h"

///Socket client for Waapi connection
AK::WwiseAuthoringAPI::Client my_client;



bool waapi_Connect(CurrentWwiseConnection &wwiseConnectionReturn)
{
	using namespace AK::WwiseAuthoringAPI;
	AkJson wwiseInfo;
	bool success = false;
	int my_Waapi_Port = wwiseConnectionReturn.port;
	int My_WAAPI_CLIENT_TIMEOUT_MS = wwiseConnectionReturn.timeoutMS;
	if (success = my_client.Connect("127.0.0.1", my_Waapi_Port))
	{
		//Get Wwise info
		if (success = my_client.Call(ak::wwise::core::getInfo,
			AkJson(AkJson::Type::Map),
			AkJson(AkJson::Type::Map),
			wwiseInfo))
		{
			wwiseConnectionReturn.port = my_Waapi_Port;
			wwiseConnectionReturn.Version = wwiseInfo["version"]["displayName"].GetVariant().GetString();
			wwiseConnectionReturn.connected = true;
		}
	}
	if (!success)
	{
		MessageBox(NULL, "! ERROR - Failed to Connect to Wwise. !", "Wwise Connection Status", MB_OK);
		wwiseConnectionReturn.connected = false;
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

bool waapi_GetChildrenFromGUID(const AK::WwiseAuthoringAPI::AkVariant &id,AK::WwiseAuthoringAPI::AkJson &results)
{
	using namespace AK::WwiseAuthoringAPI;
	AkJson query;
	AkJson args(AkJson::Map{
		{ "from", AkJson::Map{
			{ "id", AkJson::Array{ id } } } },
		{ "transform",
		{ AkJson::Array{ AkJson::Map{ { "select", AkJson::Array{ { "descendants" } } } } } }
		}
		});

	AkJson options(AkJson::Map{
		{ "return", AkJson::Array{
			AkVariant("id"),
			AkVariant("name"),
			AkVariant("path"),
			AkVariant("type"),
			AkVariant("parent"),
			AkVariant("childrenCount")
		} }
		});

	return my_client.Call(ak::wwise::core::object::get, args, options, results);
}

bool waapi_GetParentFromGUID(const AK::WwiseAuthoringAPI::AkVariant & id, AK::WwiseAuthoringAPI::AkJson & results)
{
	using namespace AK::WwiseAuthoringAPI;
	AkJson query;
	AkJson args(AkJson::Map{
		{ "from", AkJson::Map{
			{ "id", AkJson::Array{ id } } } },
		{ "transform",
		{ AkJson::Array{ AkJson::Map{ { "select", AkJson::Array{ { "parent" } } } } } }
		}
		});

	AkJson options(AkJson::Map{
		{ "return", AkJson::Array{
			AkVariant("id"),
			AkVariant("name"),
			AkVariant("path"),
			AkVariant("type"),
			AkVariant("parent"),
			AkVariant("childrenCount")
		} }
		});

	return my_client.Call(ak::wwise::core::object::get, args, options, results);;
}

bool waapi_GetObjectFromArgs(ObjectGetArgs & getArgs, AK::WwiseAuthoringAPI::AkJson & results)
{
	using namespace AK::WwiseAuthoringAPI;
	//Check for missing inputs
	if (getArgs.From[0] == "" || getArgs.From[1] == "")
	{
		PrintToConsole("!ERROR! - One or more required inputs are missing from GetObjectFromArgs call");
		return false;
	}
	AkVariant from0 = getArgs.From[0];
	AkVariant from1 = getArgs.From[1];
	AkVariant Akselect = getArgs.Select;

	AkJson args;

	if (getArgs.Where[0] == "" || getArgs.Where[1] == "")
	{
		args = (AkJson::Map{
			{ "from", AkJson::Map{ { from0, AkJson::Array{ from1 } } } },
			{ "transform",{ AkJson::Array
			{ AkJson::Map{ { "select",AkJson::Array{ { Akselect } } } } }, 
			} } });
	}
	else
	{
		AkVariant where0 = getArgs.Where[0];
		AkVariant where1 = getArgs.Where[1];
		args = (AkJson::Map{
			{ "from", AkJson::Map{ { from0, AkJson::Array{ from1 } } } },
			{ "transform",{ AkJson::Array
			{ { AkJson::Map{ { "select",AkJson::Array{ { Akselect } } } } },
			{ AkJson::Map{ { "where", AkJson::Array{ { where0,  AkJson::Array{ where1 } } } } } } }
			} } });
	}


//	using namespace AK::WwiseAuthoringAPI::JSONHelpers;
//	std::string argsToString = GetAkJsonString(args);
//	PrintToConsole(argsToString);

	AkJson options(AkJson::Map{
		{ "return", AkJson::Array{
			AkVariant("id"),
			AkVariant("name"),
			AkVariant("path"),
			AkVariant("type"),
			AkVariant("parent"),
			AkVariant("childrenCount")
		} }
		});

	if (!getArgs.customReturnArgs.empty())
	{
		for (auto i : getArgs.customReturnArgs)
		{
			if (i != "")
			{
				options["return"].GetArray().push_back(AkVariant(i));
			}
		}
	}

	return my_client.Call(ak::wwise::core::object::get, args, options, results);;
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
			MessageBox(NULL,"!Error! Malformed Results array","Waapi Results Array Error", MB_OK);
			return;
		}
	} break;
	default:
		MessageBox(NULL, "!Error! Results array is not of type AkJson::Map", "Waapi Results Array Error", MB_OK);
		return;
	}
}

void waapi_HELPER_Print_AkJson_Array(AK::WwiseAuthoringAPI::AkJson::Array & printResults)
{
		using namespace AK::WwiseAuthoringAPI::JSONHelpers;
		std::string argsToString = GetAkJsonString(printResults);
		PrintToConsole(argsToString);
}

void waapi_HELPER_Print_AkJson_Map(AK::WwiseAuthoringAPI::AkJson::Map & printResults)
{
	using namespace AK::WwiseAuthoringAPI::JSONHelpers;
	std::string argsToString = GetAkJsonString(printResults);
	PrintToConsole(argsToString);
}
