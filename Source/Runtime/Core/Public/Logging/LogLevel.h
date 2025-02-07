#pragma once

#include "Misc/EnumClassFlags.h"
#include "Platform/Platform.h"

#include <cstdint>

namespace Bloodshot
{
	enum class ELogLevel : uint8_t
	{
		Fatal = 0b000001,
		Error = 0b000010,
		Warning = 0b000100,
		Info = 0b001000,
		Debug = 0b010000,
		Trace = 0b100000,
		Count = 6,
		Mask = 15,
		All = 63
	};

	BS_DEFINE_ENUM_CLASS_FLAGS(ELogLevel);

	NODISCARD FORCEINLINE constexpr const char* LogLevelToFormat(const ELogLevel Level)
	{
		switch (Level)
		{
			case ELogLevel::Fatal: FALLTHROUGH;
			case ELogLevel::Error: FALLTHROUGH;
			case ELogLevel::Warning: return "%s[%s]: %s\033[0m\n";
			case ELogLevel::Info: FALLTHROUGH;
			case ELogLevel::Debug: FALLTHROUGH;
			case ELogLevel::Trace: return "%s[%s]: \033[0m%s\n";
			default: return "Unknown";
		}
	}

	NODISCARD FORCEINLINE constexpr const char* LogLevelToColorCode(const ELogLevel Level)
	{
		switch (Level)
		{
			case ELogLevel::Fatal: FALLTHROUGH;
			case ELogLevel::Error: return "\033[91m";
			case ELogLevel::Warning: return "\033[93m";
			case ELogLevel::Info: return "\033[94m";
			case ELogLevel::Debug: return "\033[92m";
			case ELogLevel::Trace: return "\033[97m";
			default: return "Unknown";
		}
	}

	NODISCARD FORCEINLINE constexpr const char* LogLevelToString(const ELogLevel Level)
	{
		switch (Level)
		{
			case ELogLevel::Fatal: return "FATAL";
			case ELogLevel::Error: return "ERROR";
			case ELogLevel::Warning: return "WARNING";
			case ELogLevel::Info: return "INFO";
			case ELogLevel::Debug: return "DEBUG";
			case ELogLevel::Trace: return "TRACE";
			default: return "Unknown";
		}
	}
}
