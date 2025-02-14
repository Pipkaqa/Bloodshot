#ifdef BS_PROFILING_ON

#include "Profiling/Profiler.h"
#include "Containers/Pair.h"
#include "Misc/AssertionMacros.h"
#include "Misc/FileIO.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	FORCEINLINE static FString DemangleFunctionName(FStringView MangledName)
	{
#ifdef BS_MSVC
		size_t NestedTemplates = 0;
		size_t ParamsBracketIndex = 0;

		for (size_t i = 0; i < MangledName.size(); ++i)
		{
			const char CurrentChar = MangledName[i];

			if (CurrentChar == '<')
			{
				++NestedTemplates;
			}
			else if (CurrentChar == '>')
			{
				--NestedTemplates;
			}
			else if (CurrentChar == '(' && !NestedTemplates)
			{
				ParamsBracketIndex = i;
				break;
			}
		}

		size_t PrettyFunctionNameBeginIndex = 0;

		for (size_t i = ParamsBracketIndex - 1; i > 0; --i)
		{
			const char CurrentChar = MangledName[i];

			if (CurrentChar == '<')
			{
				++NestedTemplates;
			}
			else if (CurrentChar == '>')
			{
				--NestedTemplates;
			}
			else if (!isalnum(CurrentChar) && CurrentChar != ':' && !NestedTemplates)
			{
				PrettyFunctionNameBeginIndex = i + 1;
				break;
			}
		}

		FStringView FunctionName = MangledName.substr(PrettyFunctionNameBeginIndex, ParamsBracketIndex - PrettyFunctionNameBeginIndex);
		if (FunctionName.empty())
			return FString(MangledName);
		return FString(FunctionName);
#else
		return FString(MangledName);
#endif
	}

	FProfiler& FProfiler::GetInstance()
	{
		static FProfiler Instance;
		return Instance;
	}

	void FProfiler::WriteFunctionProfile(FStringView Name, const std::chrono::milliseconds Duration, const bool bMangled)
	{
		if (!IsSessionStarted()) return;

		static size_t FunctionUniqueID = 0;

		for (TPair<const size_t, FFunctionProfile>& ProfilePair : FunctionProfiles)
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

		FunctionProfiles.emplace(++FunctionUniqueID, std::move(Profile));
	}

	void FProfiler::BeginSession()
	{
		BS_ASSERT(!bSessionStarted, "FProfiler::BeginSession: Trying to start already started profiling session");

		IFileIO::CreateIfNotExists("Logs");

		OutputStream.open("Logs/Profiler.txt");

		BS_ASSERT(OutputStream.is_open(), "FProfiler::BeginSession: Failed to open output file");

		bSessionStarted = true;
	}

	void FProfiler::EndSession()
	{
		using std::chrono::milliseconds;
		using std::chrono::microseconds;

		BS_ASSERT(bSessionStarted, "Attempting end not started Profiling Session");

		OutputStream << "[-] - Not sorted:\n";

		for (TPair<const size_t, FFunctionProfile>& ProfilePair : FunctionProfiles)
		{
			const size_t ProfileUniqueID = ProfilePair.first;
			FFunctionProfile& Profile = ProfilePair.second;

			const size_t TotalExecutions = Profile.TotalExecutions;
			const milliseconds TotalExecutionDurationMilli = Profile.TotalExecutionDuration;
			const microseconds TotalExecutionDurationMicro = std::chrono::duration_cast<microseconds>(TotalExecutionDurationMilli);
			const float AverageExecutionDurationMilli = (float)TotalExecutionDurationMilli.count() / (float)TotalExecutions;
			const float AverageExecutionDurationMicro = (float)TotalExecutionDurationMicro.count() / (float)TotalExecutions;
			Profile.AverageExecutionDurationMilli = AverageExecutionDurationMilli;

			const FString& DemangledFunctionName = Profile.bMangled ? DemangleFunctionName(Profile.Name) : FString(Profile.Name);

			// BSTODO: Paddings
			OutputStream << std::format("\t[{}] - [{}]: calls: {}; total: {}; average: {}ms / {}us\n\n",
				ProfileUniqueID,
				DemangledFunctionName,
				TotalExecutions,
				TotalExecutionDurationMilli,
				AverageExecutionDurationMilli,
				AverageExecutionDurationMicro);
		}

		std::vector<TPair<size_t, FFunctionProfile>> SortedByMs(FunctionProfiles.begin(), FunctionProfiles.end());

		std::sort(SortedByMs.begin(), SortedByMs.end(),
			[](const TPair<size_t, FFunctionProfile>& Lhs, const TPair<size_t, FFunctionProfile>& Rhs)
			{
				return Lhs.second.AverageExecutionDurationMilli > Rhs.second.AverageExecutionDurationMilli;
			});

		OutputStream << "[-] - Sorted by average in ms:\n";

		for (const TPair<const size_t, FFunctionProfile>& ProfilePair : SortedByMs)
		{
			const size_t ProfileUniqueID = ProfilePair.first;
			const FFunctionProfile& Profile = ProfilePair.second;
			const FString& DemangledFunctionName = Profile.bMangled ? DemangleFunctionName(Profile.Name) : FString(Profile.Name);
			const size_t TotalExecutions = Profile.TotalExecutions;
			const milliseconds TotalExecutionDurationMilli = Profile.TotalExecutionDuration;
			const float AverageExecutionDurationMilli = Profile.AverageExecutionDurationMilli;

			// BSTODO: Paddings
			OutputStream << std::format("\t[{}] - [{}]: calls: {}; total: {}; average: {}ms / {}us\n\n",
				ProfileUniqueID,
				DemangledFunctionName,
				TotalExecutions,
				TotalExecutionDurationMilli,
				AverageExecutionDurationMilli,
				AverageExecutionDurationMilli * 1000.f);
		}

		FunctionProfiles.clear();
		OutputStream.close();
	}
}

#endif
