#pragma once

#ifdef BS_PROFILING_ON
#include "Timer.h"

namespace Bloodshot
{
	class FProfileTimer final : public FTimer
	{
	public:
		FProfileTimer(const char* RangeName, const bool bFunctionSignaturePassed);
		~FProfileTimer();

	private:
		const char* RangeName;
		const bool bFunctionSignaturePassed;
	};
}
#endif
