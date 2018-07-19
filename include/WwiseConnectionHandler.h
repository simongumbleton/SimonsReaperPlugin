#pragma once

#include <string>
#include <map>
#include "WaapiFunctions.h"

struct WwiseObject
{
	std::map<char,std::string> properties;
	std::string name;
	std::string type;
	std::string path;
	std::string guid;
	std::string notes;
	std::string workunit;
};

void ConnectToWwise(bool suppressOuputMessages, int port);

void GetSelectedWwiseObjects(bool suppressOuputMessages);

void GetChildrenFromSelectedParent(bool suppressOuputMessages);

void GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results);

