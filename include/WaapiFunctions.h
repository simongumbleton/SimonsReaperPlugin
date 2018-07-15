#pragma once

#include "stdlib.h"
#include <string>
#include <unordered_map>

#include <AkAutobahn\Client.h>
#include <AkAutobahn\AkJson.h>

class WaapiFunctions
{
public:
	///////////////////////////////
	////     Default constructor
	///////////////////////////////
	WaapiFunctions();
	~WaapiFunctions() = default;
	WaapiFunctions(const WaapiFunctions&) = delete;
	WaapiFunctions &operator=(const WaapiFunctions&) = delete;

	///////////////////////////////
	////     Connect to wwise client
	///////////////////////////////
	bool Connect(bool suppressOuputMessages = false);
	///Socket client for Waapi connection
	AK::WwiseAuthoringAPI::Client m_client;
	int WAAPI_CLIENT_TIMEOUT_MS = 2000;
	int g_Waapi_Port = 8095;
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
	bool GetSelectedWwiseObjects(AK::WwiseAuthoringAPI::AkJson &resultsOut,bool getNotes = false);


	///////////////////////////////
	////     Get the results array for calls to Waapi functions
	///////////////////////////////
	///get the array for a succesfull call to any of the above functions, results is 'resultsOut' from above functions
	void GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array &arrayIn, AK::WwiseAuthoringAPI::AkJson &results);


private:
	//Window id's
	int m_statusTextId;
	int m_wwiseViewId;
	int m_transferWindowId;

};