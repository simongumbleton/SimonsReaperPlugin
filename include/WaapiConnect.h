#pragma once
#pragma once

#include "stdlib.h"
#include <string>

#include <AkAutobahn\Client.h>
#include <AkAutobahn\AkJson.h>



class WAAPIConnect
{
public:
	//WAAPIConnect(HWND window, int treeId, int statusTextid, int transferWindowId);
	WAAPIConnect();
	~WAAPIConnect() = default;

	WAAPIConnect(const WAAPIConnect&) = delete;
	WAAPIConnect &operator=(const WAAPIConnect&) = delete;

	//connect to wwise client
	bool Connect(bool suppressOuputMessages = false);

	//object owns this hwnd
	HWND hwnd;

	//Socket client for Waapi connection
	AK::WwiseAuthoringAPI::Client m_client;
	int WAAPI_CLIENT_TIMEOUT_MS = 2000;
	int g_Waapi_Port = 8095;

	//struct to hold the current Waapi connection info (version, port etc)
	struct CurrentWwiseConnection {
		int port;
		std::string Version;
		std::string DisplayName;
	};


private:
	//Window id's
	int m_statusTextId;
	int m_wwiseViewId;
	int m_transferWindowId;

	//The progress window will set and reset this value when it opens and closes
	std::atomic<HWND> m_progressWindow;
};