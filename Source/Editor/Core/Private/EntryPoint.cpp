#include "Editor.h"

#include <CmdParser.h>

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	std::vector<std::string> Options = {"Project"};
	Shared::FCmdParser CmdParser(Argc, Argv, Options);

	CmdParser.Parse();

	if (!CmdParser.HasAllOptions())
	{
		CmdParser.BuildErrorMessage();
		printf(CmdParser.GetErrorMessage().c_str());
		BS_TERMINATE();
	}

	const std::filesystem::path& ProjectPath = CmdParser.GetOptionValue(0);

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
