#include "EngineState.h"

#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	FEngineState::FEngineState()
	{
		Instance = this;
	}

	void FEngineState::Init()
	{
		BS_LOG(Debug, "Creating EngineState...");
	}

	void FEngineState::Dispose()
	{
		BS_LOG(Debug, "Destroying EngineState...");
	}
}
