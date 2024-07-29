#pragma once

#include "Core/EngineFramework.h"
#include "Timer.h"
#include "Utility/ISingleton.h"

#include <fstream>
#include <map>

namespace Bloodshot
{
	class ProfileTimer final : public Timer
	{
	public:
		ProfileTimer(const std::string_view functionName, const bool clean);
		~ProfileTimer();

	private:
		const std::string m_FunctionName;

		const bool m_Clean;
	};

	class Profiler final : IStaticSingleton<Profiler>
	{
		ENTRY_POINT_MODULE;

	public:
		NODISCARD FORCEINLINE static bool SessionStarted()
		{
			return s_Instance->m_SessionStarted;
		}

	private:
		std::ofstream m_Output;

		std::map<size_t, std::tuple<std::string, size_t, float, bool>> m_RangeProfileWrites;

		bool m_SessionStarted = false;

		static void BeginSession();
		static void EndSession();

		static void Write(const std::string_view functionName, const float duration, const bool clean);

		friend class ProfileTimer;
	};
}

#ifdef BS_PROFILING_ON
#define BS_PROFILE_BASE(RangeName, Clean) ::Bloodshot::ProfileTimer __profileTimer__(RangeName, Clean)
#else
#define BS_PROFILE_BASE(RangeName, Clean) ((void)0)
#endif

#define BS_PROFILE_RANGE(RangeName) BS_PROFILE_BASE(RangeName, true)
#define BS_PROFILE_FUNCTION() BS_PROFILE_BASE(BS_FUNC_SIG, false)
