#ifdef BS_PROFILING_ON
#include "Profiling/Profiler.h"
#include "AssertionMacros.h"
#include "FileIO.h"
#include "Platform.h"

namespace Bloodshot
{
	FORCEINLINE static void EraseSubstrings(std::string& OutString,
		const char* Substring,
		const size_t SubstringLength = 1)
	{
		for (size_t Position = OutString.find(Substring);
			Position != std::string::npos;
			Position = OutString.find(Substring))
		{
			OutString.erase(Position, SubstringLength);
		}
	}

	FORCEINLINE static void EraseRange(std::string& OutString,
		const char* BeginSubstring,
		const char* EndSubstring,
		const size_t RangeEndSubstringLength = 1)
	{
		size_t BeginPosition = OutString.find(BeginSubstring);
		size_t EndPosition = OutString.find(EndSubstring);

		if (BeginPosition != std::string::npos && EndPosition != std::string::npos)
		{
			OutString.erase(BeginPosition, EndPosition - BeginPosition + RangeEndSubstringLength);
		}
	}

	FORCEINLINE static void EraseRangeFromBegin(std::string& OutString, const char* EndSubstring, const size_t EndSubstringLength = 1)
	{
		size_t EndPosition = OutString.find(EndSubstring);

		if (EndPosition != std::string::npos)
		{
			OutString.erase(0, EndPosition + EndSubstringLength);
		}
	}

	FORCEINLINE static void ReplaceSubstrings(std::string& OutString,
		const char* SubstringToReplace,
		const char* NewSubstring,
		const size_t SubstringLength = 1)
	{
		for (size_t Position = OutString.find(SubstringToReplace);
			Position != std::string::npos;
			Position = OutString.find(SubstringToReplace))
		{
			OutString.replace(Position, SubstringLength, NewSubstring);
		}
	}

	FORCEINLINE static void ReplaceRange(std::string& OutString,
		const char* BeginSubstring,
		const char* EndSubstring,
		const char* NewSubstring)
	{
		const size_t BeginPosition = OutString.find(BeginSubstring);
		const size_t EndPosition = OutString.find(EndSubstring);

		if (BeginPosition != std::string::npos && EndPosition != std::string::npos)
		{
			OutString.replace(BeginPosition, EndPosition + 1, NewSubstring);
		}
	}

	FProfiler::FProfiler()
	{
		Instance = this;
	}

	void FProfiler::BeginSession()
	{
		bool& bSessionStarted = Instance->bSessionStarted;

		BS_ASSERT(!bSessionStarted, "Attempting to start already started Profiling Session");

		IFileIO::CreateIfNotExists("Logs");

		std::ofstream& Output = Instance->OutputFileStream;

		Output.open("Logs/Profiler.txt");

		BS_ASSERT(Output.is_open(), "Failed to open Profiler Output File");

		bSessionStarted = true;
	}

	void FProfiler::EndSession()
	{
		BS_ASSERT(Instance->bSessionStarted, "Attempting end not started Profiling Session");

		std::ofstream& Output = Instance->OutputFileStream;

		FUniqueIDRangeProfileMap& RangeProfilesMap = Instance->RangeProfilesMap;

		std::string CleanedRangeName;

		for (const FUniqueIDRangeProfileMap::value_type& ProfilePair : RangeProfilesMap)
		{
			const FRangeProfileInfo& ProfileInfo = ProfilePair.second;

			CleanedRangeName = std::get<0>(ProfileInfo);

			if (std::get<1>(ProfileInfo))
			{
				ReplaceRange(CleanedRangeName, "(", ")", "()");

				EraseSubstrings(CleanedRangeName, "class ", 6);
				EraseSubstrings(CleanedRangeName, "Bloodshot::", 11);
				EraseSubstrings(CleanedRangeName, "Test::", 6);
				EraseSubstrings(CleanedRangeName, "__cdecl ", 8);

				ReplaceSubstrings(CleanedRangeName, " *", "* ", 2);
				ReplaceSubstrings(CleanedRangeName, ",>", ">", 2);
				ReplaceSubstrings(CleanedRangeName, " >", ">", 2);
				ReplaceSubstrings(CleanedRangeName, " ,", ", ", 2);

				EraseSubstrings(CleanedRangeName, "void* ", 6);
				EraseSubstrings(CleanedRangeName, "void ", 5);
				EraseSubstrings(CleanedRangeName, "const ", 6);
				EraseSubstrings(CleanedRangeName, "char* ", 6);
				EraseSubstrings(CleanedRangeName, "char ", 5);

				EraseRangeFromBegin(CleanedRangeName, "* ", 2);
				EraseRangeFromBegin(CleanedRangeName, "> ", 2);
			}

			const size_t TotalRangeExecutionCalls = std::get<2>(ProfileInfo);

			const float TotalRangeExecutionDuration = std::get<3>(ProfileInfo);
			const float AverageRangeExecutionDuration = TotalRangeExecutionDuration / TotalRangeExecutionCalls;

			// BSTODO: Paddings
			Instance->OutputFileStream <<
				std::format("[{0}] - [{1}]: calls: {2}; total: {3} ms; average: {4} ms; micro: {5} us\n\n",
					ProfilePair.first,
					CleanedRangeName,
					TotalRangeExecutionCalls,
					TotalRangeExecutionDuration,
					AverageRangeExecutionDuration,
					AverageRangeExecutionDuration * 1000.f);
		}

		RangeProfilesMap.clear();
		Output.close();
	}

	void FProfiler::WriteRangeProfile(const char* Name, const float Duration, const bool bFunctionSignaturePassed)
	{
		static size_t RangeUniqueID = 0;

		FUniqueIDRangeProfileMap& RangeProfiles = Instance->RangeProfilesMap;

		for (FUniqueIDRangeProfileMap::iterator It = RangeProfiles.begin(); It != RangeProfiles.end(); ++It)
		{
			FRangeProfileInfo& ProfileInfo = It->second;

			if (std::get<0>(ProfileInfo) == Name)
			{
				std::get<2>(ProfileInfo)++;
				std::get<3>(ProfileInfo) += Duration;

				return;
			}
		}

		RangeProfiles.emplace(++RangeUniqueID, std::make_tuple(Name, bFunctionSignaturePassed, 1, Duration));
	}
}
#endif
