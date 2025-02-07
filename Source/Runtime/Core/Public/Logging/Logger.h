#pragma once

#include "Logging/LogLevel.h"
#include "Misc/Casts.h"
#include "Misc/EnumClassFlags.h"
#include "Misc/FileIO.h"
#include "Misc/LocalTime.h"
#include "Platform/Platform.h"

#include <fstream>

namespace Bloodshot
{
	class FLogger final
	{
		friend class FEngineLoop;

	public:
		NODISCARD FORCEINLINE bool IsSessionStarted() noexcept
		{
			return bSessionStarted;
		}

		template<ELogLevel Level, typename... ArgTypes>
		void Log(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			static_assert(Level != ELogLevel::Count && Level != ELogLevel::Mask && Level != ELogLevel::All,
				"Bad LogLevel passed");

#ifdef BS_LOGGING_ON
			const FString& FormattedString = std::format(Format, std::forward<ArgTypes>(Args)...);
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
			if constexpr (Level == ELogLevel::Fatal)
			{
#ifdef BS_DEBUG
				BS_DEBUG_BREAK();
#endif
				BS_TERMINATE();
			}
		}

	private:
		ELogLevel CurrentLogLevelFlags = ELogLevel::All;

		std::ofstream OutputFileStream = {};

		bool bSessionStarted = false;
		bool bAlwaysWriteToOutputFile = false;

		void BeginSession(const ELogLevel Level, const EFileOpenMode OutputFileOpenMode, const bool bAlwaysWriteToFile = false);
		void EndSession();

		FORCEINLINE void ConsoleLog(const char* Message, const char* Format, const char* ColorCode, const char* LogLevelInString)
		{
			printf(Format, ColorCode, LogLevelInString, Message);
		}
	};

	inline FLogger GLogger;
}
