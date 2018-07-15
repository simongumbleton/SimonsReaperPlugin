#pragma once

#include "stdlib.h"
#include <string>
#include <unordered_map>

#include <AkAutobahn\Client.h>
#include <AkAutobahn\AkJson.h>

///struct to hold the current Waapi connection info (version, port etc)
struct CurrentWwiseConnection {
	int port = 8095;
	std::string Version;
	int timeoutMS = 2000;
};

	///////////////////////////////
	////     Connect to wwise client
	///////////////////////////////
	bool waapi_Connect(CurrentWwiseConnection &wwiseConnectionReturn);

	///////////////////////////////
	////     Get Selected Objects
	///////////////////////////////
	///Get Seleted Objects
	bool waapi_GetSelectedWwiseObjects(AK::WwiseAuthoringAPI::AkJson &resultsOut,bool getNotes = false);

	///////////////////////////////
	////     Get Object by GUID
	///////////////////////////////

	bool waapi_GetChildrenFromGUID(const AK::WwiseAuthoringAPI::AkVariant &id,AK::WwiseAuthoringAPI::AkJson &results);

	///////////////////////////////
	////     Get the results array for calls to Waapi functions
	///////////////////////////////
	///get the array for a succesfull call to any of the above functions, results is 'resultsOut' from above functions
	void waapi_GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array &arrayIn, AK::WwiseAuthoringAPI::AkJson &results);

