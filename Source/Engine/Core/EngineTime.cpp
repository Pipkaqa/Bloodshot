#include "EngineTime.h"

#include "Benchmark/Profiler.h"
#include "Debug/Logger.h"

namespace Bloodshot
{
	void EngineTime::Init()
	{
		BS_DEBUG("Creating engine time...");
	}

	void EngineTime::Dispose()
	{
		BS_DEBUG("Destroying engine time...");
	}

	void EngineTime::Tick()
	{
		BS_PROFILE_FUNCTION();

		auto current = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> duration = current - m_StartTime;

		m_GlobalTime = duration.count();
	}
}
