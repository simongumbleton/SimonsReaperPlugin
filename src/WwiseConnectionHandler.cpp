#include <windows.h>
#include <iostream> 
#include <sstream>

#include "WwiseConnectionHandler.h"
#include "WaapiFunctions.h"

void ConnectToWwise()
{
	using namespace AK::WwiseAuthoringAPI;
	waapi_Connect();  //Connect to Wwise. Optionally pass bool true to supress message boxes from wwise connection
}

void GetSelectedWwiseObjects()
{
	if (!waapi_Connect(true))
	{
		/// WWise connection not found!
		MessageBox(NULL, "Wwise Connection not found! Exiting!", "Wwise Connection Error", MB_OK);
		return;
	}
	using namespace AK::WwiseAuthoringAPI;
	AkJson RawReturnResults;
	waapi_GetSelectedWwiseObjects(RawReturnResults, true);

	AkJson::Array MyReturnResults;
	waapi_GetWaapiResultsArray(MyReturnResults, RawReturnResults);

	for (const auto &result : MyReturnResults)
	{
		const std::string wwiseObjectGuid = result["id"].GetVariant().GetString();
		const std::string wwiseObjectName = result["name"].GetVariant().GetString();
		const std::string wwiseObjectType = result["type"].GetVariant().GetString();
		//				const std::string wwiseObjectParent = result["parent"].GetVariant().GetString();

		//create a status text string and set it
		std::stringstream status;
		status << wwiseObjectGuid + " Named: " + wwiseObjectName;
		status << " is type " + wwiseObjectType;
		//				status << " Parent =  " + wwiseObjectParent;
		std::string SelectedObject = status.str();

		MessageBox(NULL, SelectedObject.c_str(), "Wwise Objects Selected", MB_OK);
	}
}
