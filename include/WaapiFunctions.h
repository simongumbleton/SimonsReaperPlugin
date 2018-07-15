#pragma once

#include "stdlib.h"
#include <string>
#include <unordered_map>

#include <AkAutobahn\Client.h>
#include <AkAutobahn\AkJson.h>

//class WaapiFunctions
//{
//public:
	///////////////////////////////
	////     Default constructor
	///////////////////////////////
	//WaapiFunctions();
	//~WaapiFunctions() = default;
	//WaapiFunctions(const WaapiFunctions&) = delete;
	//WaapiFunctions &operator=(const WaapiFunctions&) = delete;

	///////////////////////////////
	////     Connect to wwise client
	///////////////////////////////
	bool waapi_Connect(bool suppressOuputMessages = false);

	///struct to hold the current Waapi connection info (version, port etc)
	//struct CurrentWwiseConnection {
	//	int port;
	//	std::string Version;
	//	std::string DisplayName;
	//};

	///////////////////////////////
	////     Get Selected Objects
	///////////////////////////////
	///Get Seleted Objects
	bool waapi_GetSelectedWwiseObjects(AK::WwiseAuthoringAPI::AkJson &resultsOut,bool getNotes = false);


	///////////////////////////////
	////     Get the results array for calls to Waapi functions
	///////////////////////////////
	///get the array for a succesfull call to any of the above functions, results is 'resultsOut' from above functions
	void waapi_GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array &arrayIn, AK::WwiseAuthoringAPI::AkJson &results);

//};