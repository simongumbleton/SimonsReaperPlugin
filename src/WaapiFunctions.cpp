#include <iostream> 
#include <sstream>
#include <windows.h>

#include "WaapiFunctions.h"
#include <AK/WwiseAuthoringAPI/AkVariantBase.h>
#include <AkAutobahn\AkJson.h>
#include <AK/WwiseAuthoringAPI/waapi.h>


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
		}
	}
	if (!success)
	{
		MessageBox(NULL, "! Failed to Connect !", "Wwise Connection Status", MB_OK);
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

	AkVariant from0 = getArgs.From[0];
	AkVariant from1 = getArgs.From[1];

	AkJson args(AkJson::Map
	{
		{ "from", AkJson::Map{{ from0, AkJson::Array{from1}}} },
		{ "transform",
			{ AkJson::Array{ AkJson::Map{ 
				{ "select", AkJson::Array{ 
					{ getArgs.Select } }
				}}}}
		},
		{ "where",
			{AkJson::Array{AkJson::Map{
				{ getArgs.Where[0], AkJson::Array{
					{ getArgs.Where[1]}}
				}}}}
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
			assert(!"Not implemented.");
			return;
		}
	} break;
	default:
		assert(!"Not implemented.");
		return;
	}
}
