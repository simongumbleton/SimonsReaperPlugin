#include <windows.h>
#include <iostream> 
#include <sstream>
#include <vector>


#include "WwiseConnectionHandler.h"

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

	ObjectGetArgs myGetArgs;
	myGetArgs.Select = "children";
	myGetArgs.Where = { "type:isIn","Sound" };
	myGetArgs.customReturnArgs = { "notes","workunit" };


	std::vector<WwiseObject> WwiseChildren;

	for (WwiseObject SelectedObject : WwiseObjects)
	{
		
		myGetArgs.From = { "id", SelectedObject.guid };
		AkJson::Array MyReturnResults;
		GetWwiseObjects(true, myGetArgs, MyReturnResults);

		for (const auto &result : MyReturnResults)
		{
			WwiseObject child;
			child.properties.insert(std::make_pair('guid', result["id"].GetVariant().GetString()));
			child.properties.insert(std::make_pair('name', result["name"].GetVariant().GetString()));
			child.properties.insert(std::make_pair('type', result["type"].GetVariant().GetString()));
			child.properties.insert(std::make_pair('path', result["path"].GetVariant().GetString()));
			for (std::string customReturnArg : myGetArgs.customReturnArgs)
			{
			//	const char *property = customReturnArg.c_str();
			//	child.properties[*property] = result[customReturnArg].GetVariant().GetString();
			}
			WwiseChildren.push_back(child);
		}

	}
	if (!suppressOuputMessages)
	{
		std::stringstream objectList;
		objectList << "Children of Selected Wwise objects are........\n\n";
		for (WwiseObject X : WwiseChildren)
		{
			objectList << X.properties['name'] + " of type " + X.properties['type'];
			objectList << "\n";
		}
		std::string s_ChildWwiseObjects = objectList.str();
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_ChildWwiseObjects);
	}
}

void GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results)
{
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
		return;
	}
	using namespace AK::WwiseAuthoringAPI;

	AkJson MoreRawReturnResults;
	waapi_GetObjectFromArgs(getargs, MoreRawReturnResults);

	AkJson::Array MoreMyReturnResults;
	waapi_GetWaapiResultsArray(Results, MoreRawReturnResults);


	if (!suppressOuputMessages)
	{
		std::stringstream objectList;
		objectList << "Using Generic Get Call....\n";
		objectList << "Getting " + getargs.Select + " where " + getargs.Where[0] + " " + getargs.Where[1] + "\n";
		for (const auto &result : Results)
		{
			objectList << "Name = " + result["name"].GetVariant().GetString() + " \n";
			objectList << "Type = " + result["type"].GetVariant().GetString() + " \n";
			objectList << "Notes = " + result["notes"].GetVariant().GetString() + " \n";

		}
		objectList << "..Done..\n";
		std::string getResults = objectList.str();
		PrintToConsole(getResults);
	}
}
