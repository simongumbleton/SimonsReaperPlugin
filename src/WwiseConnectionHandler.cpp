#include <windows.h>
#include <iostream> 
#include <sstream>
#include <vector>


#include "WwiseConnectionHandler.h"
#include "WaapiFunctions.h"
#include "SimonsReaperPlugin.h"

void ConnectToWwise(bool suppressOuputMessages)
{
	PrintToConsole("Connecting to Wwise.......");
	waapi_Connect(suppressOuputMessages);  //Connect to Wwise. Optionally pass bool true to supress message boxes from wwise connection
}

void GetSelectedWwiseObjects(bool suppressOuputMessages)
{
	if (!waapi_Connect(true))
	{
		/// WWise connection not found!
		MessageBox(NULL, "Wwise Connection not found! Exiting!", "Wwise Connection Error", MB_OK);
		return;
	}
	PrintToConsole("Getting Selected Wwise Objects.......");
	using namespace AK::WwiseAuthoringAPI;
	AkJson RawReturnResults;
	waapi_GetSelectedWwiseObjects(RawReturnResults, true);

	AkJson::Array MyReturnResults;
	waapi_GetWaapiResultsArray(MyReturnResults, RawReturnResults);

	std::vector<WwiseObject> WwiseObjects;
	int i = 0;
	for (const auto &result : MyReturnResults)
	{
		WwiseObject obj;
		obj.guid = result["id"].GetVariant().GetString();
		obj.name = result["name"].GetVariant().GetString();
		obj.type = result["type"].GetVariant().GetString();
		obj.path = result["path"].GetVariant().GetString();
		WwiseObjects.push_back(obj);
		i++;
	}
	if (!suppressOuputMessages)
	{ 
		std::stringstream objectList;
		objectList << "Selected Wwise objects are........\n\n";
		for (WwiseObject X : WwiseObjects)
		{
			objectList << X.name + " of type " + X.type;
			objectList << "\n";
		}
		std::string s_SelectedWwiseObjects = objectList.str();
		MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_SelectedWwiseObjects);
	}
	
}