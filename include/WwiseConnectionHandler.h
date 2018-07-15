#pragma once

#include <string>


struct WwiseObject
{
	std::string name;
	std::string type;
	std::string path;
	std::string guid;
};


void ConnectToWwise(bool suppressOuputMessages);

void GetSelectedWwiseObjects(bool suppressOuputMessages);



