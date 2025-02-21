#include "Logging/Logger.h"
#include "Logging/LoggingMacros.h"
#include "Memory/Memory.h"
#include "Misc/AssertionMacros.h"
#include "Profiling/Profiler.h"

namespace Bloodshot::Private
{
	struct FDebugEntry final
	{
		inline FDebugEntry()
		{
			using namespace Bloodshot;

#ifdef BS_LOGGING_ON
			FLogger& Logger = FLogger::GetInstance();
			Logger.BeginSession((ELogLevel::All), std::ios::trunc);
			BS_ASSERT(Logger.IsSessionStarted(), "FEngineLoop::FEngineLoop: Logging session not started!");
#endif
#ifdef BS_PROFILING_ON
			FProfiler& Profiler = FProfiler::GetInstance();
			Profiler.BeginSession();
			BS_ASSERT(Profiler.IsSessionStarted(), "FEngineLoop::FEngineLoop: Profiling session not started!");
#endif
		}

		inline ~FDebugEntry()
		{
			using namespace Bloodshot;

#ifdef BS_PROFILING_ON
			FProfiler::GetInstance().EndSession();
#endif
#ifdef BS_LOGGING_ON
			const FAllocationInfo& AllocationsInfo = FAllocationLogger::GetInstance().GetAllocationsInfo();

			BS_LOG(Info, "Allocated:   {}.B, {}.MB, {} Blocks",
				AllocationsInfo.AllocatedSize,
				AllocationsInfo.AllocatedSize >> 20ull,
				AllocationsInfo.AllocatedBlockCount);

			BS_LOG(Info, "Deallocated: {}.B, {}.MB, {} Blocks",
				AllocationsInfo.DeallocatedSize,
				AllocationsInfo.DeallocatedSize >> 20ull,
				AllocationsInfo.DeallocatedBlockCount);

			FAllocationLogger::GetInstance().IsMemoryLeak()
				? BS_LOG(Warning, "Memory leak detected")
				: BS_LOG(Info, "Memory leak not detected");

			FLogger::GetInstance().EndSession();
#endif
		}
	} GDebugEntry;
}

#include "EngineLoop.h"

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	Shared::FCmdParser CmdParser(Argc, Argv);
	Private::FEngineLoop EngineLoop;

	EngineLoop.PreInit(CmdParser);
	EngineLoop.Init();
	EngineLoop.Run();
	EngineLoop.Exit();
	EngineLoop.Dispose();
}
