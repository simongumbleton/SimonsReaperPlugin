#pragma once

struct config
{
	int waapiPort = 8095;
};


bool ReadConfigFile(config& outConfig);