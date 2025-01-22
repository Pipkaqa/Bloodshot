#pragma once

#ifdef BS_PROFILING_ON
#include "Templates/Containers/String.h"
#include "Timer.h"

namespace Bloodshot
{
	class FProfileTimer final : public FTimer
	{
	public:
		using Super = FTimer;

		FProfileTimer(FStringView FunctionName, const bool bMangled);
		~FProfileTimer();

	private:
		FStringView FunctionName;
		const bool bMangled;
	};
}
#endif
