#pragma once

#include "Allocators/Allocator.h"
#include "Containers/Array.h"
#include "Containers/StringFwd.h"
#include "Containers/StringView.h"
#include "Misc/CoreMisc.h"
#include "String/ParseTokens.h"
#include "Templates/IsCharRange.h"
#include "Templates/MemoryOperations.h"
#include "Templates/Template.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	template<typename InCharType, template<typename> typename InAllocatorType>
	class TString
	{
	public:
		using ElementType = InCharType;
		using AllocatorType = InAllocatorType<ElementType>;

		static_assert(std::is_base_of_v<IAllocator, AllocatorType>);

	private:
		using DataType = TArray<ElementType, AllocatorType>;
		DataType Data;

	public:
		FORCEINLINE TString() noexcept = default;
		FORCEINLINE TString(const TString& Other) noexcept = default;
		FORCEINLINE TString(TString&& Other) noexcept = default;
		FORCEINLINE TString& operator=(const TString& Other) noexcept = default;
		FORCEINLINE TString& operator=(TString&& Other) noexcept = default;
		FORCEINLINE TString(const AllocatorType& Allocator) noexcept : Data(Allocator) {}
		FORCEINLINE TString(std::initializer_list<ElementType> InitList) : Data(InitList) {}

		FORCEINLINE TString(const ElementType Char)
		{
			AppendChar(Char);
		}

		FORCEINLINE TString(const ElementType* Str)
		{
			AppendChars(Str);
		}

		FORCEINLINE TString(const ElementType* Str, const size_t Size)
		{
			AppendChars(Str, Size);
		}

		template<IsCharRange T>
		FORCEINLINE TString(T&& Range)
		{
			AppendChars(Bloodshot::GetData(Range), Bloodshot::GetSize(std::forward<T>(Range)));
		}

		FORCEINLINE ~TString()
		{
			Dispose();
		}

		FORCEINLINE TString& operator=(const ElementType Char)
		{
			Clear();
			return AppendChar(Char);
		}

		FORCEINLINE TString& operator=(const ElementType* Str)
		{
			Clear();
			return AppendChars(Str);
		}

		template<IsCharRange T>
		FORCEINLINE TString& operator=(T&& Range)
		{
			Clear();
			return Append(std::forward<T>(Range));
		}

		FORCEINLINE TString& operator+=(const ElementType Char)
		{
			return AppendChar(Char);
		}

		FORCEINLINE TString& operator+=(const ElementType* Str)
		{
			return Append(Str);
		}

		template<IsCharRange T>
		FORCEINLINE TString& operator+=(T&& Range)
		{
			return Append(std::forward<T>(Range));
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index)
		{
			return Data[Index];
		}

		NODISCARD FORCEINLINE const ElementType& operator[](const size_t Index) const
		{
			return Data[Index];
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE bool operator==(T&& Range) const noexcept
		{
			if (GetSize() != Bloodshot::GetSize(Range))
			{
				return false;
			}

			return strcmp(GetData(), Bloodshot::GetData(std::forward<T>(Range))) == 0;
		}

		NODISCARD FORCEINLINE AllocatorType& GetAllocator() noexcept
		{
			return Data.GetAllocator();
		}

		NODISCARD FORCEINLINE const AllocatorType& GetAllocator() const noexcept
		{
			return Data.GetAllocator();
		}

		NODISCARD FORCEINLINE DataType& GetCharArray() noexcept
		{
			return Data;
		}

		NODISCARD FORCEINLINE const DataType& GetCharArray() const noexcept
		{
			return Data;
		}

		NODISCARD FORCEINLINE ElementType* GetData() noexcept
		{
			return Data.GetData();
		}

		NODISCARD FORCEINLINE const ElementType* GetData() const noexcept
		{
			return Data.GetData();
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			const size_t Size = Data.GetSize();
			return Size ? Size - 1 : 0;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return Data.IsEmpty();
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const noexcept
		{
			return Data.IsValidIndex(Index);
		}

		FORCEINLINE void Init(const size_t Size, const ElementType& Element = ElementType())
		{
			Data.Init(Size, Element);
		}

		FORCEINLINE void Reserve(const size_t NewCapacity)
		{
			Data.Reserve(NewCapacity);
		}

		FORCEINLINE void Shrink()
		{
			Data.Shrink();
		}

		FORCEINLINE void Clear()
		{
			Data.Clear();
		}

		FORCEINLINE void Dispose()
		{
			Data.Dispose();
		}

		TString& AppendChar(const ElementType Char)
		{
			if (Char != 0)
			{
				const size_t Size = GetSize();
				const size_t InsertIndex = (Size > 0) ? Size - 1 : 0;
				const size_t InsertCount = (Size > 0) ? 1 : 2;

				Data.AddUninitialized(InsertCount);
				Data[InsertIndex] = Char;
				Data[InsertIndex + 1] = '\0';
			}

			return *this;
		}

		TString& AppendChars(const ElementType* const InStr, const size_t InSize)
		{
			if (!InSize)
			{
				return *this;
			}

			BS_CHECK(InStr);

			const size_t OldSize = GetSize();
			Data.AddUninitialized(InSize + (OldSize ? 0 : 1));

			ElementType* const RawData = GetData();
			const size_t OldEnd = OldSize - (OldSize ? 1 : 0);
			ElementType* const Destination = RawData + OldEnd;
			ConstructElements<ElementType>(Destination, InStr, InSize);

			ElementType* const NewEnd = RawData + GetSize();
			*NewEnd = '\0';

			return *this;
		}

		FORCEINLINE TString& AppendChars(const ElementType* Str)
		{
			return AppendChars(Str, strlen(Str));
		}

		FORCEINLINE TString& Append(const ElementType* Str)
		{
			return AppendChars(Str);
		}

		template<IsCharRange T>
		FORCEINLINE TString& Append(T&& Range)
		{
			const ElementType* RangeData = Bloodshot::GetData(Range);
			const size_t RangeSize = Bloodshot::GetSize(std::forward<T>(Range));
			return AppendChars(RangeData, RangeSize);
		}

		FORCEINLINE void RemoveAt(const size_t Index, const size_t Count)
		{
			Data.RemoveAt(Index, Count);
		}

		FORCEINLINE void RemoveAt(const size_t Index)
		{
			Data.RemoveAt(Index);
		}

		FORCEINLINE void RemoveLast()
		{
			Data.RemoveAt(GetSize() - 1);
		}

		NODISCARD FORCEINLINE size_t FindChar(const ElementType Char) const noexcept
		{
			const ElementType* const RawData = Data.GetData();
			for (size_t i = 0; i < GetSize(); ++i)
			{
				if (RawData[i] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool FindChar(const ElementType Char, size_t& OutIndex) const noexcept
		{
			OutIndex = FindChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE size_t FindLastChar(const ElementType Char) const noexcept
		{
			const ElementType* const RawData = GetData();
			for (size_t i = GetSize(); i > 0; --i)
			{
				if (RawData[i - 1] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool FindLastChar(const ElementType Char, size_t& OutIndex) const noexcept
		{
			OutIndex = FindLastChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE size_t Find(const ElementType* const SubStr) const noexcept
		{
			const ElementType* const RawData = GetData();
			const size_t SubStrSize = strlen(SubStr);
			size_t SameCharCount = 0;
			for (size_t i = 0; i < SubStrSize; ++i)
			{
				if (RawData[i] == SubStr[i])
				{
					++SameCharCount;
				}

				if (SameCharCount == SubStrSize)
				{
					return i - SubStrSize;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool Find(const ElementType* const SubStr, size_t& OutIndex) const noexcept
		{
			OutIndex = Find(SubStr);
			return OutIndex != IReservedValues::NoneIndex;
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE size_t Find(T&& Range) const noexcept
		{
			return Find(Bloodshot::GetData(std::forward<T>(Range)));
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE size_t Find(T&& Range, size_t& OutIndex) const noexcept
		{
			return Find(Bloodshot::GetData(std::forward<T>(Range)), OutIndex);
		}

		FORCEINLINE size_t ParseIntoArray(TArray<TString>& OutArray, const ElementType* const InDelimiter)
		{
			BS_CHECK(InDelimiter);
			OutArray.Clear();
			if (InDelimiter[0] != '\0')
			{
				ParseTokens(*this, InDelimiter, [&OutArray](FStringView Visited)
				{
					if (Visited.GetSize())
					{
						OutArray.EmplaceBack(Visited);
					}
				});
			}
			return OutArray.GetSize();
		}
	};

	template<typename CharType>
	struct TIsZeroConstructible<TString<CharType>>
	{
		static constexpr bool Value = true;
	};
}

namespace std
{
	template<typename InCharType, template<typename> typename InAllocatorType>
	struct std::hash<Bloodshot::TString<InCharType, InAllocatorType>>
	{
		size_t operator()(const Bloodshot::TString<InCharType, InAllocatorType>& Str) const noexcept
		{
			return std::_Hash_array_representation(Str.GetData(), Str.GetSize());
		}
	};
}
