#include "Logger.h"

#include "Core/Assert.h"
#include "Core/Locale.h"
#include "Memory/MemoryManager.h"

#include <chrono>
#include <filesystem>

namespace Bloodshot
{
	LogLevel Logger::s_LogLevel = {};

	std::ofstream Logger::s_ComplexLogFile = {};
	std::ofstream Logger::s_CoreLogFile = {};
	std::ofstream Logger::s_LogFile = {};
	std::ofstream Logger::s_LastSessionLogFile = {};

	void Logger::BeginSession()
	{
		static bool sessionStarted;

		FL_CORE_ASSERT(!sessionStarted, "An attempt to start log session twice");

		if (!std::filesystem::exists("Log"))
		{
			std::filesystem::create_directory("Log");
		}

		s_ComplexLogFile.open("Log/ComplexLog.txt", std::ios::app);
		s_CoreLogFile.open("Log/CoreLog.txt", std::ios::app);
		s_LogFile.open("Log/Log.txt", std::ios::app);
		s_LastSessionLogFile.open("Log/LastSessionLog.txt", std::ios::trunc);

		FL_CORE_ASSERT(s_ComplexLogFile.is_open() && s_CoreLogFile.is_open() && s_LogFile.is_open() && s_LastSessionLogFile.is_open(), "Log files creation failed");

		CoreFileLog(LogLevel::LOG_INFO, "Log-session started");

		sessionStarted = true;
	}

	void Logger::EndSession()
	{
		FL_CORE_ASSERT((s_ComplexLogFile.is_open() || s_CoreLogFile.is_open() || s_LogFile.is_open() || s_LastSessionLogFile.is_open()), "An attempt to close a log files, but they not opened yet");

		const auto allocatedBytes = MemoryManager::GetAllocatedBytes();
		const auto releasedBytes = MemoryManager::GetReleasedBytes();

		const auto newCalls = MemoryManager::GetNewCalls();
		const auto deleteCalls = MemoryManager::GetDeleteCalls();

		const auto memoryLeaks = (allocatedBytes != releasedBytes) || (newCalls != deleteCalls);

		//TODO: Need more details in log

		CoreFileLog(LogLevel::LOG_INFO, "Allocated Bytes:    {0}", allocatedBytes);
		CoreFileLog(LogLevel::LOG_INFO, "Allocated KBytes:   {0}", allocatedBytes / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Allocated MBytes:   {0}", allocatedBytes / 1024 / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Allocated GBytes:   {0}", allocatedBytes / 1024 / 1024 / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Allocated Blocks:   {0}", MemoryManager::GetAllocatedBlocks());

		CoreFileLog(LogLevel::LOG_INFO, "Released Bytes:     {0}", releasedBytes);
		CoreFileLog(LogLevel::LOG_INFO, "Released KBytes:    {0}", releasedBytes / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Released MBytes:    {0}", releasedBytes / 1024 / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Released GBytes:    {0}", releasedBytes / 1024 / 1024 / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Released Blocks:    {0}", MemoryManager::GetReleasedBlocks());

		CoreFileLog(LogLevel::LOG_INFO, "New calls:          {0}", newCalls);
		CoreFileLog(LogLevel::LOG_INFO, "Delete calls:       {0}", deleteCalls);

		CoreFileLog(memoryLeaks ? LogLevel::LOG_WARNING : LogLevel::LOG_INFO, "Memory leaks {0}", memoryLeaks ? "detected" : "not detected");

		CoreFileLog(LogLevel::LOG_INFO, "Total memory usage: {0}.B / {1}.MB", MemoryManager::GetTotalMemoryUsage(), MemoryManager::GetTotalMemoryUsage() / 1024 / 1024);
		CoreFileLog(LogLevel::LOG_INFO, "Average memory size allocated by new: {0}.B", MemoryManager::GetAverageMemorySizeAllocatedByNew());

		CoreFileLog(LogLevel::LOG_INFO, "Log-session ended");

		s_LogFile << "\n";

		s_ComplexLogFile.close();
		s_CoreLogFile.close();
		s_LogFile.close();
		s_LastSessionLogFile.close();
	}

	void Logger::ConsoleLog(const LogLevel level, const std::string_view message)
	{
		if (s_LogLevel > level)
		{
			return;
		}

		printf(GetFormatFromLogLevel(level), LogLevelToColorCode(level), LogLevelToString(level), message.data());
	}

	const char* Logger::LogLevelToString(const LogLevel level)
	{
		switch (level)
		{
			case LogLevel::LOG_TRACE:
				return "TRACE";
			case LogLevel::LOG_DEBUG:
				return "DEBUG";
			case LogLevel::LOG_INFO:
				return "INFO";
			case LogLevel::LOG_WARNING:
				return "WARNING";
			case LogLevel::LOG_ERROR:
				return "ERROR";
			case LogLevel::LOG_FATAL:
				return "FATAL";
			default:
				return "UNDEFINED";
		}
	}

	const char* Logger::LogLevelToColorCode(const LogLevel level)
	{
		switch (level)
		{
			case Bloodshot::LogLevel::LOG_TRACE:
				return "\x1B[97m";
			case Bloodshot::LogLevel::LOG_DEBUG:
				return "\x1B[92m";
			case Bloodshot::LogLevel::LOG_INFO:
				return "\x1B[94m";
			case Bloodshot::LogLevel::LOG_WARNING:
				return "\x1B[93m";
			case Bloodshot::LogLevel::LOG_ERROR:
				FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_FATAL:
				return "\x1B[91m";
			default:
				return "\033[0m";
		}
	}

	const char* Logger::GetFormatFromLogLevel(const LogLevel level)
	{
		switch (level)
		{
			case Bloodshot::LogLevel::LOG_TRACE:
				FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_DEBUG:
				FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_INFO:
				return "%s[%s]:\033[0m %s\n";
			case Bloodshot::LogLevel::LOG_WARNING:
				FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_ERROR:
				FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_FATAL:
				return "%s[%s]: %s\033[0m\n";
			default:
				return "\033[0m";
		}
	}
}
