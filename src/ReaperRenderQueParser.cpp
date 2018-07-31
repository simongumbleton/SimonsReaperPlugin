#include "SimonsReaperPlugin.h"
#include "ReaperRenderQueParser.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <filesystem>

using namespace std;

/////
////  This is an example of what a render que file looks like. 1st line is header, then the WAV files, then the RPP project
////
//<REAPER_PROJECT 0.1 "5.92/x64" 1532979245
//	QUEUED_RENDER_OUTFILE "E:\Projects\Personal\ReaperAPI\TestReaperProject\Media\TestReaperProject1.wav" 3
//	QUEUED_RENDER_OUTFILE "E:\Projects\Personal\ReaperAPI\TestReaperProject\Media\TestReaperProject2.wav" 3
//	QUEUED_RENDER_OUTFILE "E:\Projects\Personal\ReaperAPI\TestReaperProject\Media\TestReaperProject3.wav" 3
//	QUEUED_RENDER_OUTFILE "E:\Projects\Personal\ReaperAPI\TestReaperProject\Media\TestReaperProject4.wav" 3
//	QUEUED_RENDER_OUTFILE "E:\Projects\Personal\ReaperAPI\TestReaperProject\Media\TestReaperProject5.wav" 3
//	QUEUED_RENDER_OUTFILE "E:\Projects\Personal\ReaperAPI\TestReaperProject\Media\TestReaperProject6.wav" 3
//	QUEUED_RENDER_ORIGINAL_FILENAME E : \Projects\Personal\ReaperAPI\TestReaperProject\TestReaperProject.rpp
//	RIPPLE 0
//	GROUPOVERRIDE 1 0 1





void TestReadRenderQue()
{
	string resourcePath = GetReaperResourcePath();
	string QrenderPath = resourcePath + "\\QueuedRenders";
	std::vector<string> renderQueFiles;

	for (const auto & p : filesystem::directory_iterator(QrenderPath))
	{
		filesystem::path resourceFile = p.path();
		string s_resourceFile = resourceFile.string();
		if (s_resourceFile.find(".rpp") != s_resourceFile.npos)
		{
			renderQueFiles.push_back(s_resourceFile);
		}
	}
	for (auto RenderQueFile : renderQueFiles)
	{
		ParseRenderQueFile(RenderQueFile);
	}
}


void ParseRenderQueFile(string pathToQueFile)
{
	std::vector<string> RenderFiles;
	string RPPfile;

	bool done = false;
	ifstream file;
	//file.open("E:\\Projects\\Personal\\ReaperAPI\\SimonsReaperPlugin\\qrender_TestReaperProject.rpp");
	file.open(pathToQueFile);
	/// Looking for QUEUED_RENDER_OUTFILE
	string line;

	getline(file, line);	//Read the first line here, its always <REAPER_PROJECT 0.1 "5.92/x64" 1532979245 or similar

	while (!done && getline(file, line))
	{
		stringstream tkns(line);
		if (line.find("QUEUED_RENDER_OUTFILE") != line.npos)
		{
			string word;
			while (tkns >> word)
			{
				if (word.find(".wav") != line.npos)
				{
					//Found a render output file!
					word.erase(std::remove(word.begin(), word.end(), '"'), word.end());	// Removing "" from the result
					RenderFiles.push_back(word);
					//PrintToConsole(line);
				}
			}
		}
		else
		{
			if (line.find("QUEUED_RENDER_ORIGINAL_FILENAME") != line.npos)
			{
				string rppfile;
				while (tkns >> rppfile)
				{
					if (rppfile.find(".rpp") != line.npos)
					{
						RPPfile = rppfile;
					}
				}
			}
			else { done = true; break; }
		}
	}

	PrintToConsole("Reaper Render Items...");
	for (auto file : RenderFiles)
	{
		PrintToConsole(file);
	}
	PrintToConsole(RPPfile + "\n");

}