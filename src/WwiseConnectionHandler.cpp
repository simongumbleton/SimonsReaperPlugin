#include <windows.h>
#include <iostream> 
#include <sstream>
#include <vector>


#include "WwiseConnectionHandler.h"
#include "WaapiFunctions.h"
#include "SimonsReaperPlugin.h"

CurrentWwiseConnection MyCurrentWwiseConnection;


void ReportConnectionError(CurrentWwiseConnection attemptedConnection)
{
	std::string errorLog = "Wwise Connection not found on port " + std::to_string(attemptedConnection.port) + "...Exiting!";
	PrintToConsole(errorLog);
}

void ConnectToWwise(bool suppressOuputMessages, int port)
{
	
	MyCurrentWwiseConnection.port = port;

	if (waapi_Connect(MyCurrentWwiseConnection))  //Connect to Wwise. Optionally pass bool true to supress message boxes from wwise connection
	{
		if (!suppressOuputMessages)
		{
			//create a status text string and set it
			std::stringstream status;
			status << "Connected on port " + std::to_string(MyCurrentWwiseConnection.port) + ": ";
			status << " - " + MyCurrentWwiseConnection.Version;
			std::string WwiseConnectionStatus = status.str();
			//MessageBox(NULL, WwiseConnectionStatus.c_str(), "Wwise Connection Status", MB_OK);
			PrintToConsole(WwiseConnectionStatus);
		}
	}
	else
	{
		ReportConnectionError(MyCurrentWwiseConnection);
	}
}

void GetSelectedWwiseObjects(bool suppressOuputMessages)
{

	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
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
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_SelectedWwiseObjects);
	}
}

void GetChildrenFromSelectedParent(bool suppressOuputMessages)
{
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
		return;
	}
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


	ObjectGetArgs myGetArgs;
	myGetArgs.From = { "id", WwiseObjects[0].guid };
	myGetArgs.Select = "descendants";
	myGetArgs.Where = {"type:isIn","Sound"};
	myGetArgs.customReturnArgs = {"notes","workunit"};

	AkJson MoreRawReturnResults;
	waapi_GetObjectFromArgs(myGetArgs, MoreRawReturnResults);

	AkJson::Array MoreMyReturnResults;
	waapi_GetWaapiResultsArray(MoreMyReturnResults, MoreRawReturnResults);
	std::stringstream objectList;
	objectList << "Using Generic Get Call....\n";
	objectList << "Getting "+myGetArgs.Select + " where " + myGetArgs.Where[0] + " " +myGetArgs.Where[1]+"\n";
	for (const auto &result : MoreMyReturnResults)
	{
		objectList << "Name = " + result["name"].GetVariant().GetString() + " \n";
		objectList << "Type = " + result["type"].GetVariant().GetString() + " \n";
		objectList << "Notes = " + result["notes"].GetVariant().GetString() + " \n";

	}
	objectList << "..Done..\n";
	std::string getResults = objectList.str();
	PrintToConsole(getResults);


}

void GetWwiseObjects()
{
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
		return;
	}

}
