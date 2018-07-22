
#include <iostream> 
#include <sstream>
#include <vector>


#include "WwiseConnectionHandler.h"

#include "SimonsReaperPlugin.h"
#include "PluginWindow.h"


static PluginWindow myPluginWindow = PluginWindow();

WwiseConnectionHandler::WwiseConnectionHandler()
{
	
}

WwiseConnectionHandler::~WwiseConnectionHandler()
{
}

void WwiseConnectionHandler::ReportConnectionError(CurrentWwiseConnection attemptedConnection)
{
	std::string errorLog = "Wwise Connection not found on port " + std::to_string(attemptedConnection.port) + "...Exiting!";
	PrintToConsole(errorLog);
}

bool WwiseConnectionHandler::StartGUI(HINSTANCE &myhInst)
{
	myPluginWindow.SetupPluginParent(this);

	if (myPluginWindow.CreatePluginWindow(myhInst, NULL, "Hello World", 0) == 0)
	{
		
		return true;	
	}
	else
	{	
		return false;	// Somethig Failed in the window creation
	};
}

void WwiseConnectionHandler::handle_GUI_notifications(int message)
{
	switch (message)
	{
	case CONNECT:
		ConnectToWwise(false, MyCurrentWwiseConnection.port);
		break;
	default:
		break;
	}

}

bool WwiseConnectionHandler::handle_GUI_Connect()
{
	return ConnectToWwise(false, MyCurrentWwiseConnection.port);
}

bool WwiseConnectionHandler::ConnectToWwise(bool suppressOuputMessages, int port)
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
		return true;
	}
	else
	{
		ReportConnectionError(MyCurrentWwiseConnection);
		return false;
	}
}

void WwiseConnectionHandler::GetSelectedWwiseObjects(bool suppressOuputMessages)
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
		obj.properties.insert(std::make_pair("guid", result["id"].GetVariant().GetString()));
		obj.properties.insert(std::make_pair("name", result["name"].GetVariant().GetString()));
		obj.properties.insert(std::make_pair("type", result["type"].GetVariant().GetString()));
		obj.properties.insert(std::make_pair("path", result["path"].GetVariant().GetString()));
		WwiseObjects.push_back(obj);
		i++;
	}
	if (!suppressOuputMessages)
	{ 
		std::stringstream objectList;
		objectList << "Selected Wwise objects are........\n\n";
		for (WwiseObject X : WwiseObjects)
		{
			objectList << X.properties["name"] + " of type " + X.properties["type"];
			objectList << "\n";
		}
		std::string s_SelectedWwiseObjects = objectList.str();
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_SelectedWwiseObjects);
	}
}

void WwiseConnectionHandler::GetChildrenFromSelectedParent(bool suppressOuputMessages)
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
		obj.properties["guid"] = result["id"].GetVariant().GetString();
		WwiseObjects.push_back(obj);
		i++;
	}

	ObjectGetArgs myGetArgs;
	myGetArgs.Select = "children";
	//myGetArgs.Where = { "type:isIn","Sound" };
	myGetArgs.customReturnArgs = { "notes","","owner" };


	std::vector<WwiseObject> WwiseChildren;

	for (WwiseObject SelectedObject : WwiseObjects)
	{
		
		myGetArgs.From = { "id", SelectedObject.properties["guid"] };
		AkJson::Array MyReturnResults;
		GetWwiseObjects(true, myGetArgs, MyReturnResults);

		//waapi_HELPER_Print_AkJson_Array(MyReturnResults);

		for (const auto &result : MyReturnResults)
		{

			WwiseObject child;
			child.properties.insert(std::make_pair("guid", result["id"].GetVariant().GetString()));
			child.properties.insert(std::make_pair("name", result["name"].GetVariant().GetString()));
			child.properties.insert(std::make_pair("type", result["type"].GetVariant().GetString()));
			child.properties.insert(std::make_pair("path", result["path"].GetVariant().GetString()));
			for (std::string customReturnArg : myGetArgs.customReturnArgs)
			{
				if (customReturnArg != "") 
				{
					if (result.HasKey(customReturnArg.c_str()))
					{
						child.properties.insert(std::make_pair(customReturnArg.c_str(), result[customReturnArg].GetVariant().GetString()));
					}
					
				}
				
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
			objectList << X.properties["name"] + " of type " + X.properties["type"];
			objectList << "\n";
		}
		std::string s_ChildWwiseObjects = objectList.str();
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_ChildWwiseObjects);
	}
}

void WwiseConnectionHandler::GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results)
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


