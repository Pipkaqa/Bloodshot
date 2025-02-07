#include "Misc/EngineState.h"
#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	FEngineState::FEngineState()
	{
		Instance = this;
	}

	bool FEngineState::IsRunning()
	{
		return Instance->bRunning;
	}

	bool FEngineState::IsSimulating()
	{
		return Instance->bSimulating;
	}

	void FEngineState::Init()
	{
		BS_LOG(Debug, "Creating FEngineState...");
	}

	void FEngineState::Dispose()
	{
		BS_LOG(Debug, "Destroying FEngineState...");
	}
}
