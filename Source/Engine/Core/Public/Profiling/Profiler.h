#pragma once

#include "Platform.h"
#include "Templates/Singleton.h"

#include <fstream>
#include <map>

namespace Bloodshot
{
	class SProfiler final : TStaticSingleton<SProfiler>
	{
		friend class SProfileTimer;

	public:
		using FRangeProfileInfo = std::tuple<const char*, const bool, size_t, float>;
		using FUniqueIDRangeProfileMap = std::map<size_t, FRangeProfileInfo>;

		SProfiler();

		std::ofstream OutputFileStream;

		FUniqueIDRangeProfileMap RangeProfilesMap;

		bool bSessionStarted = false;

		NODISCARD FORCEINLINE static bool IsSessionStarted()
		{
			return Instance->bSessionStarted;
		}

		void BeginSession();
		void EndSession();

	private:
		static void WriteRangeProfile(const char* Name, const float Duration, const bool bFunctionSignaturePassed = false);
	};
}

