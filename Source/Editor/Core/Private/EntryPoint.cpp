#include "Editor.h"

#include <CmdParser.h>

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;
	using namespace Bloodshot::Shared;
	using namespace std::filesystem;

	std::vector<std::string> Options = {"Project"};
	FCmdParser CmdParser(Argc, Argv, Options);

	if (!CmdParser.HasAllOptions())
	{
		std::string PassedArgsMessage = "Passed: ";

		for (const std::string& Arg : CmdParser.GetPassedArgs())
		{
			PassedArgsMessage.append(Arg + " ");
		}

		printf(PassedArgsMessage.c_str());

		const std::string& ErrorMessage =
			"\nPass 1 option in any order: [Project:(Value)], other args will be ignored";

		printf(ErrorMessage.c_str());
		std::terminate();
	}

	const path& ProjectPath = CmdParser.GetOption(Options.at(0)).Value;

#ifdef BS_LOGGING_ON
	FLogger Logger;
	Logger.BeginSession((ELogLevel::All), EFileOpenMode::Truncate);
	BS_CHECK(Logger.IsSessionStarted(), "Logging Session not started!");
	{
#endif
#ifdef BS_PROFILING_ON
		FProfiler Profiler;
		Profiler.BeginSession();
		BS_CHECK(Profiler.IsSessionStarted(), "Profiling Session not started!");
		{
#endif
			BS_LOG(Trace, "Current project: {}", ProjectPath.string());
			Editor::Private::FEditor Editor(ProjectPath);
#ifdef BS_PROFILING_ON
		}
		Profiler.EndSession();
#endif
#ifdef BS_LOGGING_ON
	}
	const FAllocationInfo& AllocationsInfo = IAllocationLogger::GetAllocationsInfo();

	BS_LOG(Info, "Allocated:   {0}.B, {1}.MB, {2} Blocks",
		AllocationsInfo.AllocatedSize,
		AllocationsInfo.AllocatedSize >> 20ULL,
		AllocationsInfo.AllocatedBlockCount);

	BS_LOG(Info, "Deallocated: {0}.B, {1}.MB, {2} Blocks",
		AllocationsInfo.DeallocatedSize,
		AllocationsInfo.DeallocatedSize >> 20ULL,
		AllocationsInfo.DeallocatedBlockCount);

	IAllocationLogger::IsMemoryLeak() ? BS_LOG(Warning, "Memory leak detected") : BS_LOG(Info, "Memory leak not detected");

	Logger.EndSession();
#endif
}
