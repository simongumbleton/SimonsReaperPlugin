#include <windows.h>
#include <iostream> 
#include <sstream>
#include <vector>


#include "WwiseConnectionHandler.h"
#include "WaapiFunctions.h"
#include "SimonsReaperPlugin.h"

CurrentWwiseConnection MyCurrentWwiseConnection;


void ConnectToWwise(bool suppressOuputMessages, int port)
{
	
	MyCurrentWwiseConnection.port = port;

	if (waapi_Connect(MyCurrentWwiseConnection))  //Connect to Wwise. Optionally pass bool true to supress message boxes from wwise connection
	{
		if (!suppressOuputMessages)
		{
			StartDebugConsole();
			//create a status text string and set it
			std::stringstream status;
			status << "Connected on port " + std::to_string(MyCurrentWwiseConnection.port) + ": ";
			status << " - " + MyCurrentWwiseConnection.Version;
			std::string WwiseConnectionStatus = status.str();
			//MessageBox(NULL, WwiseConnectionStatus.c_str(), "Wwise Connection Status", MB_OK);
			PrintToConsole(WwiseConnectionStatus);
			CloseDebugConsole();
		}
	}
}

void GetSelectedWwiseObjects(bool suppressOuputMessages)
{

	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		MessageBox(NULL, "Wwise Connection not found! Exiting!", "Wwise Connection Error", MB_OK);
		return;
	}
	StartDebugConsole();
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
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_SelectedWwiseObjects);
	}
	CloseDebugConsole();
}

void GetChildrenFromSelectedParent(bool suppressOuputMessages)
{
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		MessageBox(NULL, "Wwise Connection not found! Exiting!", "Wwise Connection Error", MB_OK);
		return;
	}
	StartDebugConsole();
	PrintToConsole("Getting Children of Selected Wwise Objects.......");
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
		WwiseObjects.push_back(obj);
		i++;
	}
	std::vector<WwiseObject> WwiseChildren;

	for (WwiseObject SelectedObject : WwiseObjects)
	{
		AkJson RawReturnResults;
		waapi_GetChildrenFromGUID(SelectedObject.guid, RawReturnResults);
		AkJson::Array MyReturnResults;
		waapi_GetWaapiResultsArray(MyReturnResults, RawReturnResults);
		for (const auto &result : MyReturnResults)
		{
			WwiseObject child;
			child.guid = result["id"].GetVariant().GetString();
			child.name = result["name"].GetVariant().GetString();
			child.type = result["type"].GetVariant().GetString();
			child.path = result["path"].GetVariant().GetString();
			WwiseChildren.push_back(child);
		}

	}
	if (!suppressOuputMessages)
	{
		std::stringstream objectList;
		objectList << "Children of Selected Wwise objects are........\n\n";
		for (WwiseObject X : WwiseChildren)
		{
			objectList << X.name + " of type " + X.type;
			objectList << "\n";
		}
		std::string s_ChildWwiseObjects = objectList.str();
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_ChildWwiseObjects);
	}


	CloseDebugConsole();
}
