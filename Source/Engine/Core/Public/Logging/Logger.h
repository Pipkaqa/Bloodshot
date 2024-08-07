#pragma once

#include "EnumClassFlags.h"
#include "FileIO.h"
#include "LocalTime.h"
#include "LogLevel.h"
#include "Platform.h"

#include <fstream>

namespace Bloodshot
{
	class ILogger abstract final
	{
		friend int ::main(int Argc, char** Argv);

	public:
		static inline ELogLevel CurrentLogLevelFlags = ELogLevel::All;

		NODISCARD FORCEINLINE static bool IsSessionStarted() noexcept
		{
			return bSessionStarted;
		}

		template<ELogLevel Level, typename... ArgTypes>
		static void Log(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			static_assert(Level != ELogLevel::Count && Level != ELogLevel::Mask && Level != ELogLevel::All, "Bad LogLevel passed");

#ifdef BS_LOGGING_ON
			// BSTODO: Protect from overflow without using new
			constexpr unsigned BufferSize = 1024;
			static char Buffer[BufferSize];

			if constexpr (sizeof...(ArgTypes) > 0)
			{
				strncpy(Buffer, std::format(Format, std::forward<ArgTypes>(Args)...).c_str(), BufferSize);
			}
			else
			{
				strncpy(Buffer, Format.get().data(), BufferSize);
			}

			constexpr const char* LogLevelInString = LogLevelToString(Level);

			const char* LocalTime = ILocalTime::Now();

			if (EnumHasAllFlags(CurrentLogLevelFlags, Level))
			{
//#ifndef NDEBUG
				ConsoleLog(Buffer, LogLevelToFormat(Level), LogLevelToColorCode(Level), LogLevelInString);
//#endif
				OutputFileStream << std::format("{0} [{1}]: {2}\n", LocalTime, LogLevelInString, Buffer);
			}
			else if (bAlwaysWriteToOutputFile)
			{
				OutputFileStream << std::format("{0} [{1}]: {2}\n", LocalTime, LogLevelInString, Buffer);
			}
#endif
			if constexpr (Level == ELogLevel::Fatal)
			{
#ifndef NDEBUG
				BS_DEBUG_BREAK;
#endif
				BS_TERMINATE;
			}
		}

	private:
		static inline std::ofstream OutputFileStream = {};

		static inline bool bSessionStarted = false;
		static inline bool bAlwaysWriteToOutputFile = false;

		static void BeginSession(const ELogLevel Level, const EFileOpenMode OutputFileOpenMode, const bool bAlwaysWriteToFile = false);
		static void EndSession();

		FORCEINLINE static void ConsoleLog(const char* Message, const char* Format, const char* ColorCode, const char* LogLevelInString)
		{
			printf(Format, ColorCode, LogLevelInString, Message);
		}
	};
}
