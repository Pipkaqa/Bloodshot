#pragma once

#include "String/LowLevelString.h"
#include "String/LowLevelStringAllocator.h"
#include "Templates/IsCharRange.h"
#include "Templates/IsCharType.h"
#include "Templates/MemoryOperations.h"
#include "Templates/Template.h"
#include "Templates/TypeTraits.h"

#include <cmath>

namespace Bloodshot::Private::String
{
	template<typename InElementType>
	struct TLowLevelStringBuilder
	{
		static FLowLevelString Build(const InElementType& InElement, FLowLevelStringAllocator& Allocator)
		{
			static_assert(TFalseType<InElementType>::value, "Proved specialization of TLowLevelStringBuilder for T");
		}
	};

	template<IsIntegral T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T& InInt, FLowLevelStringAllocator& OutAllocator)
		{
			constexpr size_t BufferSize = sizeof(T) * CHAR_BIT;
			char Buffer[BufferSize];
			char* Ptr = &Buffer[BufferSize];
			bool bNegative = InInt < (T)0;
			T Temp = bNegative ? (T)-1 * InInt : InInt;
			do
			{
				*(--Ptr) = '0' + (char)(Temp % (T)10);
				Temp /= (T)10;
			} while (Temp);
			if (bNegative)
			{
				*(--Ptr) = '-';
			}
			FLowLevelString Result;
			Result.Size = (size_t)(&Buffer[BufferSize] - Ptr);
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, Ptr, Result.Size);
			return Result;
		}
	};

	template<IsFloatingPoint T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T& InFloat, FLowLevelStringAllocator& OutAllocator)
		{
			const int64_t FlooredInt = (int64_t)InFloat;
			FLowLevelString IntPartResult = TLowLevelStringBuilder<int64_t>::Build(FlooredInt, OutAllocator);
			constexpr size_t BufferSize = sizeof(T) * CHAR_BIT;
			char Buffer[BufferSize];
			char* Ptr = &Buffer[BufferSize];
			T Fraction = InFloat - (T)FlooredInt;
			if ((int64_t)std::ceil(Fraction) == 0)
			{
				*(--Ptr) = '0';
			}
			else
			{
				do
				{
					Fraction *= (T)10;
					int64_t Floored = (int64_t)Fraction;
					if (Floored == 0)
					{
						break;
					}
					*(--Ptr) = '0' + (char)Floored;
					Fraction -= Floored;

				} while (true);
			}
			*(--Ptr) = '.';
			FLowLevelString FractionPartResult;
			FractionPartResult.Size = (size_t)(&Buffer[BufferSize] - Ptr);
			FractionPartResult.Data = OutAllocator.Allocate(FractionPartResult.Size);
			MoveConstructElements(FractionPartResult.Data, Ptr, FractionPartResult.Size);
			if (FractionPartResult.Size > 2)
			{
				char* FrontSwapPtr = FractionPartResult.Data + 1;
				char* BackSwapPtr = FractionPartResult.Data + FractionPartResult.Size - 1;
				size_t Counter = 0;
				while (FrontSwapPtr < BackSwapPtr)
				{
					char Temp = *FrontSwapPtr;
					*(FrontSwapPtr++) = *BackSwapPtr;
					*(BackSwapPtr--) = Temp;
					++Counter;
					if (Counter == 5)
					{
						FractionPartResult.Size = 6;
						break;
					}
				}
			}
			return FLowLevelString(IntPartResult.Data, IntPartResult.Size + FractionPartResult.Size);
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			FLowLevelString Result;
			Result.Size = 1;
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, &InChar, Result.Size);

			return Result;
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<const T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T>::Build(InChar, OutAllocator);
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<volatile T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T>::Build(InChar, OutAllocator);
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<const volatile T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T>::Build(InChar, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<char*>
	{
		static FLowLevelString Build(const char* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			FLowLevelString Result;
			Result.Size = strlen(InStr);
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, InStr, Result.Size);

			return Result;
		}
	};

	template<>
	struct TLowLevelStringBuilder<const char*>
	{
		static FLowLevelString Build(const char* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<char*>::Build(InStr, OutAllocator);
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			FLowLevelString Result;
			Result.Size = Bloodshot::GetSize(InStr);
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, Bloodshot::GetData(InStr), Result.Size);

			return Result;
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<const T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T*>::Build(InStr, OutAllocator);
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<volatile T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T*>::Build(InStr, OutAllocator);
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<const volatile T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T*>::Build(InStr, OutAllocator);
		}
	};
}
