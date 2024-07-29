#include "Profiler.h"

#include "Core/FileIO.h"

namespace Bloodshot
{
	FORCEINLINE static void EraseSubstrings(std::string& string, const char* substring, const size_t substringLength = 1)
	{
		for (auto i = string.find(substring); i != std::string::npos; i = string.find(substring))
		{
			string.erase(i, substringLength);
		}
	}

	FORCEINLINE static void EraseRange(std::string& string, const char* first, const char* last, const size_t lastLength = 1)
	{
		size_t firstPosition = string.find(first);
		size_t lastPosition = string.find(last);

		if (firstPosition != std::string::npos && lastPosition != std::string::npos)
		{
			string.erase(firstPosition, lastPosition - firstPosition + lastLength);
		}
	}

	FORCEINLINE static void EraseRangeFromBegin(std::string& string, const char* last, const size_t lastLength = 1)
	{
		size_t lastPosition = string.find(last);

		if (lastPosition != std::string::npos)
		{
			string.erase(0, lastPosition + lastLength);
		}
	}

	FORCEINLINE static void ReplaceSubstrings(std::string& string, const char* substring, const char* newSubstring, const size_t substringLength = 1)
	{
		for (auto i = string.find(substring); i != std::string::npos; i = string.find(substring))
		{
			string.replace(i, substringLength, newSubstring);
		}
	}

	FORCEINLINE static void ReplaceRange(std::string& string, const char* first, const char* last, const char* newSubstring)
	{
		size_t firstPosition = string.find(first);
		size_t lastPosition = string.find(last);

		if (firstPosition != std::string::npos && lastPosition != std::string::npos)
		{
			string.replace(firstPosition, lastPosition + 1, newSubstring);
		}
	}

	ProfileTimer::ProfileTimer(const std::string_view functionName, const bool clean)
		: m_FunctionName(functionName), m_Clean(clean)
	{
	}

	ProfileTimer::~ProfileTimer()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> duration = endTimepoint - m_StartTimepoint;

		Profiler::Write(m_FunctionName, duration.count() * 1000, m_Clean);
	}

	void Profiler::BeginSession()
	{
		auto& sessionStarted = s_Instance->m_SessionStarted;

		BS_ASSERT(!sessionStarted, "An attempt to start already started profile session");

		FileIO::CreateIfNotExists("Logs");

		auto& output = s_Instance->m_Output;

		output.open("Logs/Profiler.txt");

		BS_ASSERT(output.is_open(), "Failed to open profiler log output file!");

		sessionStarted = true;
	}

	void Profiler::EndSession()
	{
		BS_ASSERT(s_Instance->m_SessionStarted, "An attempt end not started profile session");

		auto& output = s_Instance->m_Output;
		auto& rangeProfileWrites = s_Instance->m_RangeProfileWrites;

		std::string cleaned;

		for (const auto& [uniqueID, writes] : rangeProfileWrites)
		{
			cleaned = std::get<0>(writes);

			if (!std::get<3>(writes))
			{
				ReplaceRange(cleaned, "(", ")", "()");

				EraseSubstrings(cleaned, "class ", 6);
				EraseSubstrings(cleaned, "Bloodshot::", 11);
				EraseSubstrings(cleaned, "Test::", 6);
				EraseSubstrings(cleaned, "__cdecl ", 8);

				ReplaceSubstrings(cleaned, " *", "* ", 2);
				ReplaceSubstrings(cleaned, ",>", ">", 2);
				ReplaceSubstrings(cleaned, " >", ">", 2);
				ReplaceSubstrings(cleaned, " ,", ", ", 2);

				EraseSubstrings(cleaned, "void* ", 6);
				EraseSubstrings(cleaned, "void ", 5);
				EraseSubstrings(cleaned, "const ", 6);
				EraseSubstrings(cleaned, "char* ", 6);
				EraseSubstrings(cleaned, "char ", 5);

				EraseRangeFromBegin(cleaned, "* ", 2);
				EraseRangeFromBegin(cleaned, "> ", 2);
			}

			size_t calls = std::get<1>(writes);

			float total = std::get<2>(writes);
			float average = total / calls;

			// BSTODO: Paddings
			s_Instance->m_Output << std::format("[{0}] - [{1}]: calls: {2}; total: {3} ms; average: {4} ms; micro: {5} us\n\n",
				uniqueID,
				cleaned,
				calls,
				total,
				average,
				average * 1000);
		}

		rangeProfileWrites.clear();
		output.close();
	}

	void Profiler::Write(const std::string_view rangeName, const float duration, const bool clean)
	{
		static size_t uniqueWrites = 0;

		auto& output = s_Instance->m_Output;
		auto& rangeProfileWrites = s_Instance->m_RangeProfileWrites;

		auto it = rangeProfileWrites.begin();

		bool rangeFounded = false;

		for (; it != rangeProfileWrites.end(); ++it)
		{
			if (std::get<0>(it->second) == rangeName)
			{
				rangeFounded = true;
				break;
			}
		}

		if (rangeFounded)
		{
			auto& writes = it->second;

			std::get<1>(writes)++;
			std::get<2>(writes) += duration;

			return;
		}

		rangeProfileWrites.emplace(++uniqueWrites, std::make_tuple(rangeName, 1, duration, clean));
	}
}
