#pragma once

#include "Core/Locale.h"
#include "Utility/Utility.h"

#include <format>
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
	public:
		static void BeginSession();
		static void EndSession();

		NODISCARD FORCEINLINE static LogLevel GetLogLevel() noexcept
		{
			return s_LogLevel;
		}

		FORCEINLINE static void SetLogLevel(LogLevel level) noexcept
		{
			s_LogLevel = level;
		}

		template<typename... Args>
		static void CoreFileLog(const LogLevel level, const std::format_string<Args...> format, Args&&... args)
		{
			const auto& message = std::format(format, std::forward<Args>(args)...);

			const auto log = std::format("{0} [{1}]: {2}\n", Locale::GetLocalTime(), LogLevelToString(level), message);

			s_ComplexLogFile << log;
			s_CoreLogFile << log;
			s_LastSessionLogFile << log;

#ifndef NDEBUG
			ConsoleLog(level, message);
#endif
		}

		template<typename... Args>
		static void FileLog(const LogLevel level, const char* format, Args&&... args)
		{
			const auto& message = std::format(format, std::forward<Args>(args)...);

			const auto log = std::format("{0} [{1}]: {2}\n", Locale::GetLocalTime(), LogLevelToString(level), message);

			s_ComplexLogFile << log;
			s_LogFile << log;
			s_LastSessionLogFile << log;

#ifndef NDEBUG
			ConsoleLog(level, message);
#endif
		}

		static void ConsoleLog(const LogLevel level, const std::string_view message);

	private:
		static LogLevel s_LogLevel;

		static std::ofstream s_CoreLogFile;
		static std::ofstream s_LogFile;
		static std::ofstream s_ComplexLogFile;
		static std::ofstream s_LastSessionLogFile;

		NODISCARD static const char* LogLevelToString(const LogLevel level);
		NODISCARD static const char* LogLevelToColorCode(const LogLevel level);
		NODISCARD static const char* GetFormatFromLogLevel(const LogLevel level);

		friend class Core;
	};
}

#define FL_CORE_LOG(Level, Format, ...) ::Bloodshot::Logger::CoreFileLog(Level, Format, __VA_ARGS__)
#define FL_LOG(Level, Format, ...) ::Bloodshot::Logger::FileLog(Level, Format, __VA_ARGS__)

#define FL_CORE_TRACE(Format, ...) FL_CORE_LOG(::Bloodshot::LogLevel::LOG_TRACE, Format, __VA_ARGS__)
#define FL_CORE_DEBUG(Format, ...) FL_CORE_LOG(::Bloodshot::LogLevel::LOG_DEBUG, Format, __VA_ARGS__)
#define FL_CORE_INFO(Format, ...) FL_CORE_LOG(::Bloodshot::LogLevel::LOG_INFO, Format, __VA_ARGS__)
#define FL_CORE_WARNING(Format, ...) FL_CORE_LOG(::Bloodshot::LogLevel::LOG_WARNING, Format, __VA_ARGS__)
#define FL_CORE_ERROR(Format, ...) FL_CORE_LOG(::Bloodshot::LogLevel::LOG_ERROR, Format, __VA_ARGS__)
#define FL_CORE_FATAL(Format, ...) FL_CORE_LOG(::Bloodshot::LogLevel::LOG_FATAL, Format, __VA_ARGS__)

#define FL_TRACE(Format, ...) FL_LOG(::Bloodshot::LogLevel::LOG_TRACE, Format, __VA_ARGS__)
#define FL_DEBUG(Format, ...) FL_LOG(::Bloodshot::LogLevel::LOG_DEBUG, Format, __VA_ARGS__)
#define FL_INFO(Format, ...) FL_LOG(::Bloodshot::LogLevel::LOG_INFO, Format, __VA_ARGS__)
#define FL_WARNING(Format, ...) FL_LOG(::Bloodshot::LogLevel::LOG_WARNING, Format, __VA_ARGS__)
#define FL_ERROR(Format, ...) FL_LOG(::Bloodshot::LogLevel::LOG_ERROR, Format, __VA_ARGS__)
#define FL_FATAL(Format, ...) FL_LOG(::Bloodshot::LogLevel::LOG_FATAL, Format, __VA_ARGS__)
