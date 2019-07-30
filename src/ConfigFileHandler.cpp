#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "SimonsReaperPlugin.h"

#include <filesystem>
#include "..\include\ConfigFileHandler.h"

using namespace std;

const char dummyconfig[] = "waapiPort=8095\n";


bool ReadConfigFile(config & outConfig)
{
	string reaperPath = GetReaperResourcePath();
	string pluginPath = reaperPath + "\\UserPlugins";


	std::istringstream is_file(dummyconfig);

	std::string line;
	while (std::getline(is_file, line))
	{
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '='))
		{
			std::string value;
			if (std::getline(is_line, value))
				PrintToConsole(key + " " + value);
		}
	}
	return true;
}
