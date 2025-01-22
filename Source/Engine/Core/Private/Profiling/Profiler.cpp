#ifdef BS_PROFILING_ON
#include "Profiling/Profiler.h"
#include "AssertionMacros.h"
#include "FileIO.h"
#include "Platform/Platform.h"
#include "Templates/Containers/Pair.h"
#include "Templates/Containers/Vector.h"

namespace Bloodshot
{
	FORCEINLINE static void EraseSubstrings(FString& OutString, FStringView Substring)
	{
		for (size_t Position = OutString.find(Substring); Position != FString::npos; Position = OutString.find(Substring))
		{
			OutString.erase(Position, Substring.length());
		}
	}

	FORCEINLINE static void EraseRange(FString& OutString, FStringView BeginSubstring, FStringView EndSubstring)
	{
		const size_t BeginPosition = OutString.find(BeginSubstring);
		const size_t EndPosition = OutString.find(EndSubstring);

		if (BeginPosition != FString::npos && EndPosition != FString::npos)
		{
			OutString.erase(BeginPosition, EndPosition - BeginPosition + EndSubstring.length());
		}
	}

	FORCEINLINE static void EraseRangeFromBegin(FString& OutString, FStringView EndSubstring)
	{
		const size_t EndPosition = OutString.find(EndSubstring);

		if (EndPosition != FString::npos)
		{
			OutString.erase(0, EndPosition + EndSubstring.length());
		}
	}

	FORCEINLINE static void ReplaceSubstrings(FString& OutString,
		FStringView SubstringToReplace,
		FStringView NewSubstring)
	{
		for (size_t Position = OutString.find(SubstringToReplace);
			Position != FString::npos;
			Position = OutString.find(SubstringToReplace))
		{
			OutString.replace(Position, SubstringToReplace.length(), NewSubstring);
		}
	}

	FORCEINLINE static void ReplaceRange(FString& OutString,
		FStringView BeginSubstring,
		FStringView EndSubstring,
		FStringView NewSubstring)
	{
		const size_t BeginPosition = OutString.find(BeginSubstring);
		const size_t EndPosition = OutString.find(EndSubstring);

		if (BeginPosition != FString::npos && EndPosition != FString::npos)
		{
			OutString.replace(BeginPosition, EndPosition + 1, NewSubstring);
		}
	}

	FORCEINLINE static FString DemangleFunctionName(FStringView String)
	{
		FString Result = String.data();

		ReplaceRange(Result, "(", ")", "()");

		EraseSubstrings(Result, "class ");
		EraseSubstrings(Result, "Bloodshot::");
		EraseSubstrings(Result, "Editor::");
		EraseSubstrings(Result, "Private::");
		EraseSubstrings(Result, "Test::");
		EraseSubstrings(Result, "FResourceManager::");
		EraseSubstrings(Result, "FEngineTime::");
		EraseSubstrings(Result, "__cdecl ");

		ReplaceSubstrings(Result, " *", "* ");
		ReplaceSubstrings(Result, ",>", ">");
		ReplaceSubstrings(Result, " >", ">");
		ReplaceSubstrings(Result, " ,", ", ");

		EraseSubstrings(Result, "void* ");
		EraseSubstrings(Result, "void ");
		EraseSubstrings(Result, "const ");
		EraseSubstrings(Result, "char* ");
		EraseSubstrings(Result, "char ");

		EraseRangeFromBegin(Result, "* ");
		EraseRangeFromBegin(Result, "> ");

		return Result;
	}

	FProfiler::FProfiler()
	{
		Instance = this;
	}

	bool FProfiler::IsSessionStarted()
	{
		return Instance && Instance->bSessionStarted;
	}

	void FProfiler::BeginSession()
	{
		bool& bSessionStarted = Instance->bSessionStarted;

		BS_ASSERT(!bSessionStarted, "Attempting to start already started Profiling Session");

		IFileIO::CreateIfNotExists("Logs");

		std::ofstream& Output = Instance->OutputStream;

		Output.open("Logs/Profiler.txt");

		BS_ASSERT(Output.is_open(), "Failed to open Profiler Output File");

		bSessionStarted = true;
	}

	void FProfiler::EndSession()
	{
		using std::chrono::milliseconds;
		using std::chrono::microseconds;

		BS_ASSERT(Instance->bSessionStarted, "Attempting end not started Profiling Session");

		std::ofstream& Output = Instance->OutputStream;

		FFunctionProfileMap& Profiles = Instance->FunctionProfiles;

		Output << "[-] - Not sorted:\n";

		for (TPair<const size_t, FFunctionProfile>& ProfilePair : Profiles)
		{
			const size_t ProfileUniqueID = ProfilePair.first;
			FFunctionProfile& Profile = ProfilePair.second;

			const size_t TotalExecutions = Profile.TotalExecutions;
			const milliseconds TotalExecutionDurationMilli = Profile.TotalExecutionDuration;
			const microseconds TotalExecutionDurationMicro = std::chrono::duration_cast<microseconds>(TotalExecutionDurationMilli);
			const float AverageExecutionDurationMilli = (float)TotalExecutionDurationMilli.count() / (float)TotalExecutions;
			const float AverageExecutionDurationMicro = (float)TotalExecutionDurationMicro.count() / (float)TotalExecutions;
			Profile.AverageExecutionDurationMilli = AverageExecutionDurationMilli;

			const FString& DemangledFunctionName = DemangleFunctionName(Profile.Name);

			// BSTODO: Paddings
			Output << std::format("\t[{}] - [{}]: calls: {}; total: {}; average: {}ms / {}us\n\n",
				ProfileUniqueID,
				DemangledFunctionName,
				TotalExecutions,
				TotalExecutionDurationMilli,
				AverageExecutionDurationMilli,
				AverageExecutionDurationMicro);

		}

		TVector<TPair<size_t, FFunctionProfile>> TempProfiles(Profiles.begin(), Profiles.end());

		std::sort(TempProfiles.begin(), TempProfiles.end(),
			[](const TPair<size_t, FFunctionProfile>& Lhs, const TPair<size_t, FFunctionProfile>& Rhs)
			{
				return Lhs.second.AverageExecutionDurationMilli > Rhs.second.AverageExecutionDurationMilli;
			});

		Output << "[-] - Sorted by average in ms:\n";

		for (const TPair<const size_t, FFunctionProfile>& ProfilePair : TempProfiles)
		{
			const size_t ProfileUniqueID = ProfilePair.first;
			const FFunctionProfile& Profile = ProfilePair.second;
			const FString& DemangledFunctionName = DemangleFunctionName(Profile.Name);
			const size_t TotalExecutions = Profile.TotalExecutions;
			const milliseconds TotalExecutionDurationMilli = Profile.TotalExecutionDuration;
			const float AverageExecutionDurationMilli = Profile.AverageExecutionDurationMilli;

			// BSTODO: Paddings
			Output << std::format("\t[{}] - [{}]: calls: {}; total: {}; average: {}ms / {}us\n\n",
				ProfileUniqueID,
				DemangledFunctionName,
				TotalExecutions,
				TotalExecutionDurationMilli,
				AverageExecutionDurationMilli,
				AverageExecutionDurationMilli * 1000.f);
		}

		Profiles.clear();
		Output.close();
	}

	void FProfiler::WriteFunctionProfile(FStringView Name, const std::chrono::milliseconds Duration, const bool bMangled)
	{
		if (!IsSessionStarted()) return;

		static size_t FunctionUniqueID = 0;

		FFunctionProfileMap& Profiles = Instance->FunctionProfiles;

		for (TPair<const size_t, FFunctionProfile>& ProfilePair : Profiles)
		{
			FFunctionProfile& Profile = ProfilePair.second;

			if (Profile.Name == Name)
			{
				++Profile.TotalExecutions;
				Profile.TotalExecutionDuration += Duration;
				return;
			}
		}

		FFunctionProfile Profile;
		Profile.Name = Name;
		Profile.bMangled = bMangled;
		Profile.TotalExecutions = 1;
		Profile.TotalExecutionDuration = Duration;

		Profiles.emplace(++FunctionUniqueID, std::move(Profile));
	}
}
#endif
