#pragma once

#include "Logging/LogLevel.h"
#include "Memory/Memory.h"
#include "Misc/EnumClassFlags.h"
#include "Misc/LocalTime.h"
#include "Platform/Platform.h"
#include "String/Format.h"

#include <fstream>

namespace Bloodshot
{
	class FLogger final
	{
		friend struct Private::FDebugEntry;

	public:
		static FLogger& GetInstance();

		NODISCARD FORCEINLINE void SetLogLevelFlags(const ELogLevel LogLevelFlags) noexcept
		{
			CurrentLogLevelFlags = LogLevelFlags;
		}

		NODISCARD FORCEINLINE bool IsSessionStarted() const noexcept
		{
			return bSessionStarted;
		}

		template<typename... ArgTypes>
		void Log(const char* InFmt, const ELogLevel InLevel, ArgTypes&&... Args)
		{
#ifdef BS_LOGGING_ON
			Private::String::FLowLevelString Str = Private::String::LLFormat(InFmt, std::forward<ArgTypes>(Args)...);
			const char* const RawStr = Str.Data;

			const char* const LogLevelStr = Private::LogLevel::LogLevelToString(InLevel);
			const char* const LocalTimeStr = ILocalTime::Now();

			bool bWriteToFile = false;

			if (EnumHasAllFlags(CurrentLogLevelFlags, InLevel))
			{
				// BSTODO: Temp
				//#ifndef NDEBUG
				ConsoleLog(Private::LogLevel::LogLevelToFormat(InLevel), Private::LogLevel::LogLevelToColorCode(InLevel), LogLevelStr, RawStr);
				//#endif

				bWriteToFile = true;
			}

			if (bWriteToFile || bAlwaysWriteToFile)
			{
				Private::String::FLowLevelString OutputStr = Private::String::LLFormat("{} [{}]: {}\n", LocalTimeStr, LogLevelStr, RawStr);
				OutputStream << OutputStr.Data;
				OutputStr.Release();
			}

			Str.Release();
#endif
			if (InLevel == ELogLevel::Fatal)
			{
#ifdef BS_DEBUG
				BS_DEBUG_BREAK();
#endif
				BS_TERMINATE();
			}
		}

	private:
		ELogLevel CurrentLogLevelFlags = ELogLevel::All;
		std::ofstream OutputStream;

		bool bSessionStarted = false;
		bool bAlwaysWriteToFile = false;

		void BeginSession(const ELogLevel Level, const int OutputStreamOpenMode, const bool bAlwaysWriteToFile = false);
		void EndSession();

		FORCEINLINE void ConsoleLog(const char* Format, const char* ColorCode, const char* LogLevel, const char* Message)
		{
			printf(Format, ColorCode, LogLevel, Message);
		}
	};
}
