#pragma once

#include "CmdParser.h"
#include "HeaderTool.h"

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	std::vector<std::string> Options = {"Source", "Output"};
	Shared::FCmdParser CmdParser(Argc, Argv, Options);

	CmdParser.Parse();
	CmdParser.BuildErrorMessage();

	if (!CmdParser.HasAllOptions())
	{
		printf(CmdParser.GetErrorMessage().c_str());
		std::terminate();
	}

	const std::filesystem::path& SourcePath = CmdParser.GetOptionValue(0);
	const std::filesystem::path& OutputPath = CmdParser.GetOptionValue(1);

	HeaderTool::FHeaderTool HeaderTool(SourcePath, OutputPath);
	HeaderTool.Launch();
}
