#include "Misc/EngineState.h"

namespace Bloodshot
{
	FEngineState& FEngineState::GetInstance()
	{
		static FEngineState Instance;
		return Instance;
	}
}
