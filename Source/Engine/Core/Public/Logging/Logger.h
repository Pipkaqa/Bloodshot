#pragma once

#include "Casts.h"
#include "EnumClassFlags.h"
#include "FileIO.h"
#include "LocalTime.h"
#include "LogLevel.h"
#include "Platform/Platform.h"

#include <cstdint>
#include <fstream>

namespace Bloodshot
{
	class FLogger final
	{
	public:
		static inline ELogLevel CurrentLogLevelFlags = ELogLevel::All;

		NODISCARD FORCEINLINE static bool IsSessionStarted() noexcept
		{
			bSessionStarted = true;
			return bSessionStarted;
		}

		NODISCARD FORCEINLINE static size_t GetErrorCount()
		{
			return ErrorCount;
		}

		void BeginSession(const ELogLevel Level, const EFileOpenMode OutputFileOpenMode, const bool bAlwaysWriteToFile = false);
		void EndSession();

		template<ELogLevel Level, typename... ArgTypes>
		static void Log(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			static_assert(Level != ELogLevel::Count && Level != ELogLevel::Mask && Level != ELogLevel::All,
				"Bad LogLevel passed");

#ifdef BS_LOGGING_ON
			const std::string& FormattedString = std::format(Format, std::forward<ArgTypes>(Args)...);
			const char* const RawFormattedString = FormattedString.c_str();

			constexpr const char* LogLevelInString = LogLevelToString(Level);
			const char* LocalTime = ILocalTime::Now();

			if (EnumHasAllFlags(CurrentLogLevelFlags, Level))
			{
				// BSTODO: Temp
				//#ifndef NDEBUG
				ConsoleLog(RawFormattedString, LogLevelToFormat(Level), LogLevelToColorCode(Level), LogLevelInString);
				//#endif
				OutputFileStream << std::format("{0} [{1}]: {2}\n", LocalTime, LogLevelInString, RawFormattedString);
			}
			else if (bAlwaysWriteToOutputFile)
			{
				OutputFileStream << std::format("{0} [{1}]: {2}\n", LocalTime, LogLevelInString, RawFormattedString);
			}
#endif
			if constexpr (Level == ELogLevel::Error)
			{
				++ErrorCount;
			}
			else if constexpr (Level == ELogLevel::Fatal)
			{
#ifdef BS_DEBUG_ON
				BS_DEBUG_BREAK;
#endif
				BS_TERMINATE;
			}
		}

	private:
		static inline std::ofstream OutputFileStream = {};

		static inline bool bSessionStarted = false;
		static inline bool bAlwaysWriteToOutputFile = false;

		static inline size_t ErrorCount = 0;

		FORCEINLINE static void ConsoleLog(const char* Message, const char* Format, const char* ColorCode, const char* LogLevelInString)
		{
			printf(Format, ColorCode, LogLevelInString, Message);
		}
	};
}
