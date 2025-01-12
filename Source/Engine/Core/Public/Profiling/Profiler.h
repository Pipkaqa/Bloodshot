#pragma once

#ifdef BS_PROFILING_ON
#include "Platform/Platform.h"
#include "Templates/Singleton.h"

#include <fstream>
#include <map>

// BSTODO: Rewrite trash code

namespace Bloodshot
{
	class FProfiler final : TStaticSingleton<FProfiler>
	{
		friend class FProfileTimer;

	public:
		using FRangeProfileInfo = std::tuple<const char*, const bool, size_t, float>;
		using FUniqueIDRangeProfileMap = std::map<size_t, FRangeProfileInfo>;

		FProfiler();

		std::ofstream OutputFileStream;
		FUniqueIDRangeProfileMap RangeProfilesMap;
		bool bSessionStarted = false;

		static bool IsSessionStarted();

		void BeginSession();
		void EndSession();

	private:
		static void WriteRangeProfile(const char* Name, const float Duration, const bool bFunctionSignaturePassed = false);
	};
}
#endif
