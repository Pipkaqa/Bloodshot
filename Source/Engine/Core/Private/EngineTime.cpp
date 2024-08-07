#include "EngineTime.h"

#include "Logging/LoggingMacros.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	FEngineTime::FEngineTime()
	{
		Instance = this;
	}

	void FEngineTime::Init()
	{
		BS_LOG(Debug, "Creating EngineTime...");
	}

	void FEngineTime::Dispose()
	{
		BS_LOG(Debug, "Destroying EngineTime...");
	}

	void FEngineTime::Tick()
	{
		BS_PROFILE_FUNCTION();

		const std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<float> Duration = Now - StartTimepoint;

		GlobalTime = Duration.count();
	}
}
