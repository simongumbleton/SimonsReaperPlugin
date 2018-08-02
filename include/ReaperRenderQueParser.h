#pragma once
#include "WwiseConnectionHandler.h"


struct RenderQueJob
{
	std::string RenderQueFilePath;
	WwiseObject parentWwiseObject;
	std::vector<std::string> RenderQueJobFileList;
	std::string ParentReaperProject;
};

std::vector<RenderQueJob> ListOfRenderQueJobs;

void ParseRenderQueFile(std::string pathToQueFile);

void TestReadRenderQue();