#pragma once

#include "Timer.h"

namespace Bloodshot
{
	class SProfileTimer final : public STimer
	{
	public:
		SProfileTimer(const char* RangeName, const bool bFunctionSignaturePassed);
		~SProfileTimer();

	private:
		const char* RangeName;
		const bool bFunctionSignaturePassed;
	};
}
