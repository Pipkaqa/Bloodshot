#pragma once

#ifdef BS_PROFILING_ON

#include "Misc/Timer.h"

namespace Bloodshot
{
	class FProfileTimer final : public FTimer
	{
	public:
		using Super = FTimer;

		FProfileTimer(const char* FunctionName, const bool bMangled);
		~FProfileTimer();

	private:
		const char* FunctionName;
		const bool bMangled;
	};
}

#endif
