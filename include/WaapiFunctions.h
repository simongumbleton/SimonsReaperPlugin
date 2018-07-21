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
};

struct ObjectGetArgs {
	std::vector<std::string> From{ "","" };
	std::string Select = "";
	std::vector<std::string> Where{ "","" };
	std::vector<std::string> customReturnArgs{"","","",""};
};

/// Vectors to store GET choices
struct GetObjectChoices {
	std::vector<std::string> waapiGETchoices_FROM
	{
		"id", "search", "path", "ofType"
	};

	std::vector<std::string> waapiGETchoices_SELECT
	{
		"parent", "children", "descendants", "ancestors"
	};

	std::vector<std::string> waapiGETchoices_WHERE
	{
		"name:contains", "name:matches", "type:isIn", "category:isIn"
	};

	std::vector<std::string> waapiGETchoices_RETURN
	{
		"id", "name", "notes", "type", "shortId",
		"classId", "category", "filePath", "workunit",
		"parent", "owner", "path", "isPlayable", "childrenCount",
		"sound:convertedWemFilePath", "sound:originalWavFilePath",
		"soundbank:bnkFilePath", "music:transitionRoot", "music:playlistRoot",
		"audioSource:playbackDuration", "audioSource:maxDurationSource",
		"audioSource:trimValues", "workunit:isDefault", "workunit:type",
		"workunit:isDirty", "switchContainerChild:context"
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
	////     Get the results array for calls to Waapi functions
	///////////////////////////////
	///get the array for a succesfull call to any of the above functions, results is 'resultsOut' from above functions
	void waapi_GetWaapiResultsArray(AK::WwiseAuthoringAPI::AkJson::Array &arrayIn, AK::WwiseAuthoringAPI::AkJson &results);
