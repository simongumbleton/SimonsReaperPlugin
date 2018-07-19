#pragma once

#include <string>


struct WwiseObject
{
	std::string name;
	std::string type;
	std::string path;
	std::string guid;
};

void ConnectToWwise(bool suppressOuputMessages, int port);

void GetSelectedWwiseObjects(bool suppressOuputMessages);

void GetChildrenFromSelectedParent(bool suppressOuputMessages);

void GetWwiseObjects();

