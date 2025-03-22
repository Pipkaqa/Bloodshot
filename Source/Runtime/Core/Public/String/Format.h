#pragma once

#include "Memory/Memory.h"
#include "String/LowLevelString.h"
#include "String/LowLevelStringAllocator.h"
#include "String/LowLevelStringBuilder.h"
#include "Templates/MemoryOperations.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	namespace Private::String
	{
		template<typename ElementType>
		static void LLFormatHelper(FLowLevelString& OutStr, FLowLevelString& OutFmt, FLowLevelStringAllocator& OutAllocator, const ElementType& InElement)
		{
			char*& OutFmtData = OutFmt.Data;
			size_t& OutFmtSize = OutFmt.Size;
			int64_t OpenBracketIndex = -1;
			for (size_t i = 0; i < OutFmtSize; ++i)
			{
				if (OutFmtData[i] == '{')
				{
					OpenBracketIndex = i;
					break;
				}
			}
			if (OpenBracketIndex < 0)
			{
				return;
			}

			int64_t CloseBracketIndex = -1;
			for (size_t i = OpenBracketIndex + 1; i < OutFmtSize; ++i)
			{
				if (OutFmtData[i] == '}')
				{
					CloseBracketIndex = i;
					break;
				}
			}
			if (CloseBracketIndex < 0)
			{
				return;
			}
			char* const OutStrData = OutStr.Data;
			size_t& OutStrSize = OutStr.Size;
			const size_t FmtLengthToAppend = OpenBracketIndex;
			MoveConstructElements(OutStrData + OutStrSize, OutFmtData, FmtLengthToAppend);
			OutAllocator.Allocate(FmtLengthToAppend);
			OutStrSize += FmtLengthToAppend;
			FLowLevelString ElementStrData = TLowLevelStringBuilder<ElementType>::Build(InElement, OutAllocator);
			OutStrSize += ElementStrData.Size;
			OutFmtData += CloseBracketIndex + 1;
			OutFmtSize -= CloseBracketIndex + 1;
		}

		template<typename... ArgTypes>
		FLowLevelString LLFormatImpl(const char* InFmt, ArgTypes&&... Args)
		{
			static FLowLevelStringAllocator Allocator;
			FLowLevelString Fmt;
			const size_t& FmtSize = (Fmt.Size = strlen(InFmt));
			char* const& FmtData = (Fmt.Data = Allocator.Allocate(FmtSize));
			ConstructElements<char>(FmtData, InFmt, FmtSize);
			FLowLevelString Result;
			char* const ResultData = Allocator.Allocate(0);
			Result.Data = ResultData;
			size_t& ResultSize = Result.Size;
			(LLFormatHelper(Result, Fmt, Allocator, std::forward<ArgTypes>(Args)), ...);
			MoveConstructElements(ResultData + ResultSize, FmtData, FmtSize);
			ResultSize += FmtSize + 1;
			ResultData[ResultSize - 1] = '\0';
			Allocator.Reset();
			return Result;
		}

		template<typename... ArgTypes>
		NODISCARD FLowLevelString LLFormat(const char* const InFmt, ArgTypes&&... Args)
		{
			FLowLevelString Temp = LLFormatImpl(InFmt, std::forward<ArgTypes>(Args)...);
			const size_t TempSize = Temp.Size;
			FLowLevelString Result;
			char* const ResultData = Result.Allocate(TempSize);
			MoveConstructElements(ResultData, Temp.Data, TempSize);
			return Result;
		}
	}
}
