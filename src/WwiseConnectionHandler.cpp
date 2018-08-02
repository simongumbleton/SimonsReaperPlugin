
#include <iostream> 
#include <sstream>
#include <vector>

#include "WwiseConnectionHandler.h"

#include "SimonsReaperPlugin.h"
#include "PluginWindow.h"

#include "ReaperRenderQueParser.h"


static PluginWindow myPluginWindow = PluginWindow();
static CreateImportWindow myCreateImportWindow = CreateImportWindow();

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

bool WwiseConnectionHandler::StartGUI_Get(HINSTANCE &myhInst)
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

bool WwiseConnectionHandler::StartGUI_Transfer(HINSTANCE &myhInst)
{
	myCreateImportWindow.SetupPluginParent(this);

	if (myCreateImportWindow.CreateTransferWindow(myhInst, NULL, "Hello World", 0) == 0)
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

bool WwiseConnectionHandler::GetSelectedWwiseObjects(bool suppressOuputMessages)
{
	return false;
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

	if (!waapi_GetSelectedWwiseObjects(RawReturnResults, true))
	{
		//Something went wrong!
		PrintToConsole("ERROR. Get Selected Object Call Failed. Exiting.");
		return mySelectedObject;
	}

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

}

bool WwiseConnectionHandler::GetWwiseObjects(bool suppressOuputMessages, ObjectGetArgs& getargs, AK::WwiseAuthoringAPI::AkJson::Array& Results)
{
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
		return false;
	}
	using namespace AK::WwiseAuthoringAPI;

	if (getargs.fromSelected)	/// Need to get the selected object first
	{
		WwiseObject selectedObject = GetSelectedObject();
		getargs.From[0] = "id";
		getargs.From[1] = selectedObject.properties["id"];
	}

	AkJson MoreRawReturnResults;
	if (!waapi_GetObjectFromArgs(getargs, MoreRawReturnResults))
	{
		//Something went wrong!
		PrintToConsole("ERROR. Get Object Call Failed. Exiting.");
		return false;
	}
	waapi_GetWaapiResultsArray(Results, MoreRawReturnResults);

	if (!suppressOuputMessages)
	{
		std::stringstream objectList;
		objectList << "Getting Wwise Results....\n";
		objectList << "Getting " + getargs.Select + " from " + getargs.From[0] +" "+getargs.From[1];
		if (getargs.Where[0] != "")
			objectList << " where " + getargs.Where[0] + " " + getargs.Where[1];
		objectList << "\n";
		int resultCount = 1;
		// Data structs to hold string and number results in after translation. BOOL types get converted to strings
		std::map<std::string, std::string> stringResults;
		std::map<std::string, double> numberResults;

		for (const auto &result : Results)
		{
			for (const auto &string : getargs.customReturnArgs)
			{
				if (result.HasKey(string))
				{
					waapi_TranslateJSONResults(stringResults, numberResults, result, string);
				}
			}
			objectList << "Result " + std::to_string(resultCount) + ": " + "\n";

			for (auto i : stringResults)
			{
				objectList << i.first + " = " + i.second + "\n";
			}
			for (auto i : numberResults)
			{
				objectList << i.first + " = " + std::to_string(i.second) + "\n";
			}
			resultCount++;
		}
		objectList << "..Done..\n";
		std::string getResults = objectList.str();
		PrintToConsole(getResults);
	}
	return true;
}

bool WwiseConnectionHandler::CreateWwiseObjects(bool suppressOutputMessages, CreateObjectArgs & createArgs, AK::WwiseAuthoringAPI::AkJson::Array & Results)
{
	if (!waapi_Connect(MyCurrentWwiseConnection))
	{
		/// WWise connection not found!
		ReportConnectionError(MyCurrentWwiseConnection);
		return false;
	}
	using namespace AK::WwiseAuthoringAPI;

	//Sort the inputs  "ActorMixer","Blend", "Random", "Sequence", "Switch"			RandomOrSequence 0 or 1
	if (createArgs.Type == "Blend")
	{
		createArgs.Type = "BlendContainer";
	}
	else if (createArgs.Type == "Switch")
	{
		createArgs.Type = "SwitchContainer";
	}
	else if (createArgs.Type == "Random")
	{
		createArgs.Type = "RandomSequenceContainer";
		createArgs.RandomOrSequence = 0;
	}
	else if (createArgs.Type == "Sequence")
	{
		createArgs.Type = "RandomSequenceContainer";
		createArgs.RandomOrSequence = 1;
	}

	waapi_UndoHandler(Begin, "Create Object");

	AkJson MoreRawReturnResults;
	if (!waapi_CreateObjectFromArgs(createArgs, MoreRawReturnResults))
	{
		//Something went wrong!
		PrintToConsole("ERROR. Create Object Call Failed. Exiting.");
		waapi_UndoHandler(Cancel, "Create Object");
		return false;
	}
	waapi_GetWaapiResultsArray(Results, MoreRawReturnResults);

	waapi_UndoHandler(End, "Create Object");
	waapi_SaveWwiseProject();	
	return true;
}


