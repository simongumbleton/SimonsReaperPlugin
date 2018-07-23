
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

	std::vector<WwiseObjectStruct> WwiseObjects;
	int i = 0;
	for (const auto &result : MyReturnResults)
	{
		WwiseObjectStruct obj;
		obj.properties.insert(std::make_pair("id", result["id"].GetVariant().GetString()));
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
		for (WwiseObjectStruct X : WwiseObjects)
		{
			objectList << X.properties["name"] + " of type " + X.properties["type"];
			objectList << "\n";
		}
		std::string s_SelectedWwiseObjects = objectList.str();
		//MessageBox(NULL, s_SelectedWwiseObjects.c_str(), "Wwise Objects Selected", MB_OK);
		PrintToConsole(s_SelectedWwiseObjects);
	}
}

WwiseObject WwiseConnectionHandler::GetSelectedObject()
{
	WwiseObject mySelectedObject;
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
		return mySelectedObject;
	}
	using namespace AK::WwiseAuthoringAPI;
	AkJson RawReturnResults;
	waapi_GetSelectedWwiseObjects(RawReturnResults, true);

	AkJson::Array MyReturnResults;
	waapi_GetWaapiResultsArray(MyReturnResults, RawReturnResults);

	
	for (const auto &result : MyReturnResults)
	{
		WwiseObjectStruct obj;
		mySelectedObject.properties.insert(std::make_pair("id", result["id"].GetVariant().GetString()));
		mySelectedObject.properties.insert(std::make_pair("name", result["name"].GetVariant().GetString()));
		mySelectedObject.properties.insert(std::make_pair("type", result["type"].GetVariant().GetString()));
		mySelectedObject.properties.insert(std::make_pair("path", result["path"].GetVariant().GetString()));
	}

	return mySelectedObject;
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

	std::vector<WwiseObjectStruct> WwiseObjects;
	int i = 0;
	for (const auto &result : MyReturnResults)
	{
		WwiseObjectStruct obj;
		obj.properties["id"] = result["id"].GetVariant().GetString();
		WwiseObjects.push_back(obj);
		i++;
	}

	ObjectGetArgs myGetArgs;
	myGetArgs.Select = "children";
	//myGetArgs.Where = { "type:isIn","Sound" };
	myGetArgs.customReturnArgs = { "notes","","owner" };


	std::vector<WwiseObjectStruct> WwiseChildren;

	for (WwiseObjectStruct SelectedObject : WwiseObjects)
	{
		
		myGetArgs.From = { "id", SelectedObject.properties["id"] };
		AkJson::Array MyReturnResults;
		GetWwiseObjects(true, myGetArgs, MyReturnResults);

		//waapi_HELPER_Print_AkJson_Array(MyReturnResults);

		for (const auto &result : MyReturnResults)
		{

			WwiseObjectStruct child;
			child.properties.insert(std::make_pair("id", result["id"].GetVariant().GetString()));
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
		for (WwiseObjectStruct X : WwiseChildren)
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

	if (getargs.fromSelected)	/// Need to get the selected object first
	{
		WwiseObject selectedObject = GetSelectedObject();
		getargs.From[0] = "id";
		getargs.From[1] = selectedObject.properties["id"];
	}

	AkJson MoreRawReturnResults;
	waapi_GetObjectFromArgs(getargs, MoreRawReturnResults);

	waapi_GetWaapiResultsArray(Results, MoreRawReturnResults);


	if (!suppressOuputMessages)
	{
		std::stringstream objectList;
		objectList << "Using Generic Get Call....\n";
		objectList << "Getting " + getargs.Select + " where " + getargs.Where[0] + " " + getargs.Where[1] + "\n";
		int resultCount = 1;
		for (const auto &result : Results)
		{
			for (const auto &string : getargs.customReturnArgs)
			{
				if (result.HasKey(string))
				{
					objectList << "Result " + std::to_string(resultCount) + ": " + string + " = ";

					using namespace AK::WwiseAuthoringAPI::JSONHelpers;
					std::string argsToString = GetAkJsonString(result);


					AK::WwiseAuthoringAPI::AkJson::Type type;
					type = result[string].GetType();
					if (type == AK::WwiseAuthoringAPI::AkJson::Type::Variant)
						objectList << result[string].GetVariant().GetString();
					else if (type == AK::WwiseAuthoringAPI::AkJson::Type::Map)
						for (const auto x : result[string].GetMap())
						{
						//	objectList << x.first + " = " + x.second.GetVariant().GetString();
						}

					else if (type == AK::WwiseAuthoringAPI::AkJson::Type::Array)
						objectList << "";// result[string].GetArray().operator[string];
					else
						objectList << "Ak Type not found";
					objectList << "\n";
				}
				
			}
			resultCount++;
		}
		objectList << "..Done..\n";
		std::string getResults = objectList.str();
		PrintToConsole(getResults);
	}
}


