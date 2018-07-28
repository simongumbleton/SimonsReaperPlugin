#pragma once

#include "stdlib.h"
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <AkAutobahn\Client.h>
#include <AkAutobahn\AkJson.h>

///struct to hold the current Waapi connection info (version, port etc)
struct CurrentWwiseConnection {
	int port = 8095;
	std::string Version;
	int timeoutMS = 2000;
	bool connected = false;
	bool supressDebugOutput = false;
};

struct ObjectGetArgs {	/// Init struct with some sensible defaults that can be error checked
	std::vector<std::string> From{ "","" };
	std::string Select = "descendants";
	std::vector<std::string> Where{"",""};
	std::vector<std::string> customReturnArgs{};
	bool fromSelected = false;
};

struct CreateObjectArgs {
	std::string ParentID = "";
	std::string Type = "";
	std::string Name = "";
	std::string onNameConflict = "merge";
	std::string Notes = "";
};

/// Vectors to store GET choices
struct GetObjectChoices {
	std::vector<std::string> waapiGETchoices_FROM
	{
		"Wwise Selection","id", "search", "path", "ofType"
	};

	std::vector<std::string> waapiGETchoices_SELECT
	{
		"descendants", "parent", "children", "ancestors"
	};

	std::vector<std::string> waapiGETchoices_WHERE
	{
		"","name:contains", "name:matches", "type:isIn", "category:isIn"
	};

	std::vector<std::string> waapiGETchoices_RETURN
	{
		"id", "name", "type", "path", "parent", "notes",
		"shortId","classId", "category", "filePath",
		"owner",  "isPlayable", "childrenCount",
		"sound:convertedWemFilePath", "sound:originalWavFilePath",
		"soundbank:bnkFilePath", "music:transitionRoot", "music:playlistRoot",
		"audioSource:playbackDuration", "audioSource:maxDurationSource",
		"audioSource:trimValues", "workunit:isDefault", "workunit:type",
		"workunit:isDirty", "switchContainerChild:context"
	};
};

struct CreateObjectChoices {
	std::vector<std::string> waapiCREATEchoices_TYPE
	{
		"ActorMixer","Blend", "Random", "Sequence", "Switch"
	};
	std::vector<std::string> waapiCREATEchoices_NAMECONFLICT
	{
		"merge","rename", "replace", "fail"
	};
	std::vector<std::string> waapiCREATEchoices_IMPORTOP
	{
		"useExisting","createNew", "replaceExisting"
	};

};

	///////////////////////////////
	////     Connect to wwise client
	///////////////////////////////
	bool waapi_Connect(CurrentWwiseConnection &wwiseConnectionReturn);

	///////////////////////////////
	////     Get Selected Objects
	///////////////////////////////
	///Get Seleted Objects
	bool waapi_GetSelectedWwiseObjects(AK::WwiseAuthoringAPI::AkJson &resultsOut,bool getNotes = false);

	///////////////////////////////
	////     Get Object by GUID
	///////////////////////////////

	bool waapi_GetChildrenFromGUID(const AK::WwiseAuthoringAPI::AkVariant &id,AK::WwiseAuthoringAPI::AkJson &results);

	bool waapi_GetParentFromGUID(const AK::WwiseAuthoringAPI::AkVariant &id, AK::WwiseAuthoringAPI::AkJson &results);

	bool waapi_GetObjectFromArgs(ObjectGetArgs & getArgs, AK::WwiseAuthoringAPI::AkJson & results);

	///////////////////////////////
	////    Create and Import
	///////////////////////////////
	bool waapi_CreateObjectFromArgs(CreateObjectArgs & createArgs, AK::WwiseAuthoringAPI::AkJson & results);

	bool wappi_ImportFromArgs();

	///////////////////////////////
	////     Get the results array for calls to Waapi functions
	///////////////////////////////
	///get the array for a succesfull call to any of the above functions, results is 'resultsOut' from above functions
	void waapi_GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array &arrayIn, AK::WwiseAuthoringAPI::AkJson &results);

	///////////////////////////////
	////     Helper function to print JSON data in a more readable way
	///////////////////////////////
	void waapi_HELPER_Print_AkJson_Array(AK::WwiseAuthoringAPI::AkJson::Array &printResults);
	void waapi_HELPER_Print_AkJson_Map(AK::WwiseAuthoringAPI::AkJson::Map &printResults);

	///////////////////////////////
	////     Helper function to Get the Results arrays in a more useful way than AkJson
	///////////////////////////////
	bool waapi_TranslateJSONResults(std::map<std::string,std::string> &INstringResults, std::map<std::string, double> &INnumberResults, AK::WwiseAuthoringAPI::AkJson result, std::string stringKey);

