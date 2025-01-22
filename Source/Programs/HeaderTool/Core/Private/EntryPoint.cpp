#pragma once

#include "CmdParser.h"
#include "HeaderTool.h"

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;
	using namespace Bloodshot::HeaderTool;
	using namespace Bloodshot::Shared;
	using namespace std::filesystem;

	std::vector<std::string> Args = {"Source:C:/Users/Pipka/Desktop/Bloodshot/Source/Engine",
	"Output:C:/Users/Pipka/Desktop/Bloodshot/Intermediate/Engine"};

	std::vector<std::string> Options = {"Source", "Output"};
	FCmdParser CmdParser((int)Args.size(), Args, Options);

	if (!CmdParser.HasAllOptions())
	{
		std::string PassedArgsMessage = "Passed: ";

		for (const std::string& Arg : CmdParser.GetPassedArgs())
		{
			PassedArgsMessage.append(Arg + " ");
		}

		printf(PassedArgsMessage.c_str());

		const std::string& ErrorMessage =
			"\nPass 2 options in any order: [Source:(Value)] and [Output:(Value)], other args will be ignored";

		printf(ErrorMessage.c_str());
		std::terminate();
	}

	const path& SourcePath = path(CmdParser.GetOption(Options.at(0)).Value);
	const path& OutputPath = path(CmdParser.GetOption(Options.at(1)).Value);

	Bloodshot::HeaderTool::FHeaderTool HeaderTool(OutputPath);

	if (SourcePath.has_extension() && SourcePath.extension() == ".h")
	{
		HeaderTool.ProcessHeaderFile(SourcePath);
	}
	else
	{
		HeaderTool.ProcessHeaderFilesRecursive(SourcePath);
	}
}
