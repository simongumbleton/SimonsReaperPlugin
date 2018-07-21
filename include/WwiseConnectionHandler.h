#pragma once

#include <string>
#include <map>
#include "WaapiFunctions.h"

class WwiseConnectionHandler
{
public:
	WwiseConnectionHandler();
	~WwiseConnectionHandler();

	struct WwiseObject
	{
		std::map<std::string, std::string> properties;
	};

	CurrentWwiseConnection MyCurrentWwiseConnection;

	void ConnectToWwise(bool suppressOuputMessages, int port);

	void GetSelectedWwiseObjects(bool suppressOuputMessages);

	void GetChildrenFromSelectedParent(bool suppressOuputMessages);

	void GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results);


private:

	void ReportConnectionError(CurrentWwiseConnection attemptedConnection);

};






