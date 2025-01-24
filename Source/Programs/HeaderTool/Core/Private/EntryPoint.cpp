#pragma once

#include "HeaderTool.h"

#include <CmdParser.h>

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	std::vector<std::string> Options = {"Source", "Output", "Module", "OutputToSingleFiles"};
	Shared::FCmdParser CmdParser(Argc, Argv, Options);

	CmdParser.Parse();
	CmdParser.BuildErrorMessage();

	if (!CmdParser.HasAllOptions())
	{
		printf(CmdParser.GetErrorMessage().c_str());
		std::terminate();
	}

	const std::string& OutputToSingleFilesStr = CmdParser.GetOptionValue(3);
	bool bOutputToSingleFiles;

	if (OutputToSingleFilesStr == "Yes")
	{
		bOutputToSingleFiles = true;
	}
	else if (OutputToSingleFilesStr == "No")
	{
		bOutputToSingleFiles = false;
	}
	else
	{
		printf(CmdParser.GetErrorMessage().c_str());
		std::terminate();
	}

	const std::filesystem::path& SourcePath = CmdParser.GetOptionValue(0);
	const std::filesystem::path& OutputPath = CmdParser.GetOptionValue(1);
	const std::string& Module = CmdParser.GetOptionValue(2);

	Bloodshot::HeaderTool::FHeaderTool HeaderTool(SourcePath, OutputPath, Module, bOutputToSingleFiles);
	HeaderTool.Launch();
}
