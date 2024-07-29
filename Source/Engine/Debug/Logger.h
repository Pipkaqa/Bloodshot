#pragma once

#include "Core/FileIO.h"
#include "Core/LocalTime.h"

#include <fstream>

namespace Bloodshot
{
	enum class LogLevel : uint8_t
	{
		LOG_TRACE = 0,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
		LOG_FATAL,
		LOG_NONE
	};

	class Logger abstract final
	{
		ENTRYPOINT;

	public:
		struct Config
		{
			LogLevel m_LogLevel = LogLevel::LOG_INFO;

			FileIO::FileOpenMode m_OutputFileOpenMode = FileIO::FileOpenMode::Truncate;
		};

		NODISCARD FORCEINLINE static const Config& GetConfig() noexcept
		{
			return s_Config;
		}

		NODISCARD FORCEINLINE static bool SessionStarted() noexcept
		{
			return s_SessionStarted;
		}

		NODISCARD FORCEINLINE static LogLevel GetLogLevel() noexcept
		{
			return s_Config.m_LogLevel;
		}

		FORCEINLINE static void SetLogLevel(const LogLevel level) noexcept
		{
			s_Config.m_LogLevel = level;
		}

		template<typename... Args>
		static void Log(const LogLevel level, const std::format_string<Args...> format, Args&&... args)
		{
			const auto& message = std::format(format, std::forward<Args>(args)...);

			s_Output << std::format("{0} [{1}]: {2}\n", LocalTime::GetLocalTime(), LogLevelToString(level), message);

#ifndef NDEBUG
			ConsoleLog(level, message);
#endif
		}

	private:
		static Config s_Config;
		static std::ofstream s_Output;
		static bool s_SessionStarted;

		static void BeginSession(const Config& config);
		static void EndSession();

		FORCEINLINE static void ConsoleLog(const LogLevel level, const std::string_view message)
		{
			if (s_Config.m_LogLevel > level) return;

			printf(LogLevelToFormat(level), LogLevelToColorCode(level), LogLevelToString(level), message.data());
		}

		NODISCARD static const char* LogLevelToString(const LogLevel level);
		NODISCARD static const char* LogLevelToColorCode(const LogLevel level);
		NODISCARD static const char* LogLevelToFormat(const LogLevel level);
	};
}

#ifdef BS_LOGGING_ON
#define BS_LOG(Level, Format, ...) ::Bloodshot::Logger::Log(Level, Format, __VA_ARGS__)
#else
#define BS_LOG(Level, Format, ...) ((void)0)
#endif

#define BS_TRACE(Format, ...) BS_LOG(::Bloodshot::LogLevel::LOG_TRACE, Format, __VA_ARGS__)
#define BS_DEBUG(Format, ...) BS_LOG(::Bloodshot::LogLevel::LOG_DEBUG, Format, __VA_ARGS__)
#define BS_INFO(Format, ...) BS_LOG(::Bloodshot::LogLevel::LOG_INFO, Format, __VA_ARGS__)
#define BS_WARNING(Format, ...) BS_LOG(::Bloodshot::LogLevel::LOG_WARNING, Format, __VA_ARGS__)
#define BS_ERROR(Format, ...) BS_LOG(::Bloodshot::LogLevel::LOG_ERROR, Format, __VA_ARGS__)
#define BS_FATAL(Format, ...) BS_LOG(::Bloodshot::LogLevel::LOG_FATAL, Format, __VA_ARGS__)