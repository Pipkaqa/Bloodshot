#include "EngineLoop.h"

namespace Bloodshot
{
	FEngineLoop::FEngineLoop()
	{
		IAllocationLogger::ClearLog();

#ifdef BS_LOGGING_ON
		Logger.BeginSession((ELogLevel::All), EFileOpenMode::Truncate);
		BS_ASSERT(Logger.IsSessionStarted(), "Logging Session not started!");
#endif
#ifdef BS_PROFILING_ON
		Profiler.BeginSession();
		BS_ASSERT(Profiler.IsSessionStarted(), "Profiling Session not started!");
#endif
	}

	FEngineLoop::~FEngineLoop()
	{
#ifdef BS_PROFILING_ON
		Profiler.EndSession();
#endif
#ifdef BS_LOGGING_ON
		const FAllocationInfo& AllocationsInfo = IAllocationLogger::GetAllocationsInfo();

		BS_LOG(Info, "Allocated:   {}.B, {}.MB, {} Blocks",
			AllocationsInfo.AllocatedSize,
			AllocationsInfo.AllocatedSize >> 20ULL,
			AllocationsInfo.AllocatedBlockCount);

		BS_LOG(Info, "Deallocated: {}.B, {}.MB, {} Blocks",
			AllocationsInfo.DeallocatedSize,
			AllocationsInfo.DeallocatedSize >> 20ULL,
			AllocationsInfo.DeallocatedBlockCount);

		IAllocationLogger::IsMemoryLeak() ? BS_LOG(Warning, "Memory leak detected") : BS_LOG(Info, "Memory leak not detected");
		Logger.EndSession();
#endif
	}

	void FEngineLoop::PreInit(Shared::FCmdParser& CmdParser)
	{
		// BSTODO: Load config

		bool bEditor = true;

		if (bEditor)
		{
			EngineContext = new FEngineEditorContext;
		}
		else
		{
			EngineContext = new FEngineGameContext;
		}

		EngineContext->LoadConfig(CmdParser);
		EngineContext->PreInit();
	}

	void FEngineLoop::Init()
	{
		EngineContext->Init();
	}

	void FEngineLoop::Run()
	{
		EngineContext->Run();
	}

	void FEngineLoop::Exit()
	{
		EngineContext->Exit();
	}

	void FEngineLoop::Dispose()
	{
		EngineContext->Dispose();
		delete EngineContext;
	}
}
