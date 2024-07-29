#include "Logger.h"

#include "Memory/MemoryManager.h"

namespace Bloodshot
{
	Logger::Config Logger::s_Config = {};
	std::ofstream Logger::s_Output = {};
	bool Logger::s_SessionStarted = false;

	void Logger::BeginSession(const Config& config)
	{
		auto& started = s_SessionStarted;

		BS_ASSERT(!started, "An attempt to start already started log session");

		s_Config = config; 

		FileIO::CreateIfNotExists("Logs");

		auto& output = s_Output;

		output.open("Logs/Logger.txt", SafeCast<int>(s_Config.m_OutputFileOpenMode));

		BS_ASSERT(output.is_open(), "Failed to open logger output log file!");
		BS_INFO("Log-session started");

		started = true;
	}

	void Logger::EndSession()
	{
		BS_ASSERT(s_SessionStarted, "An attempt end not started log session");

		s_Output.close();

		BS_INFO("Log-session ended");
	}

	const char* Logger::LogLevelToString(const LogLevel level)
	{
		switch (level)
		{
			case LogLevel::LOG_TRACE: return "TRACE";
			case LogLevel::LOG_DEBUG: return "DEBUG";
			case LogLevel::LOG_INFO: return "INFO";
			case LogLevel::LOG_WARNING: return "WARNING";
			case LogLevel::LOG_ERROR: return "ERROR";
			case LogLevel::LOG_FATAL:return "FATAL";
		}

		return "UNDEFINED";
	}

	const char* Logger::LogLevelToColorCode(const LogLevel level)
	{
		switch (level)
		{
			case Bloodshot::LogLevel::LOG_TRACE: return "\033[97m";
			case Bloodshot::LogLevel::LOG_DEBUG: return "\033[92m";
			case Bloodshot::LogLevel::LOG_INFO: return "\033[94m";
			case Bloodshot::LogLevel::LOG_WARNING: return "\033[93m";
			case Bloodshot::LogLevel::LOG_ERROR: FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_FATAL: return "\033[91m";
		}

		return "UNDEFINED";
	}

	const char* Logger::LogLevelToFormat(const LogLevel level)
	{
		switch (level)
		{
			case Bloodshot::LogLevel::LOG_TRACE: FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_DEBUG: FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_INFO: return "%s[%s]: \033[0m%s\n";
			case Bloodshot::LogLevel::LOG_WARNING: FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_ERROR: FALLTHROUGH;
			case Bloodshot::LogLevel::LOG_FATAL: return "%s[%s]: %s\033[0m\n";
		}

		return "UNDEFINED";
	}
}
