#pragma once

#include "Allocators/Allocator.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Casts.h"
#include "Platform/Platform.h"
#include "Templates/MemoryOperations.h"

namespace Bloodshot
{
	class FBitReference
	{
	public:
		FORCEINLINE explicit FBitReference(uint64_t& Data, uint64_t Mask)
			: Data(Data)
			, Mask(Mask)
		{
		}

		FORCEINLINE FBitReference& operator=(const FBitReference& Copy)
		{
			*this = (bool)Copy;
			return *this;
		}

		FORCEINLINE void operator=(const bool Value)
		{
			if (Value)
			{
				Data |= Mask;
			}
			else
			{
				Data &= ~Mask;
			}
		}

		FORCEINLINE void operator|=(const bool Value)
		{
			if (Value)
			{
				Data |= Mask;
			}
		}

		FORCEINLINE void operator&=(const bool Value)
		{
			if (!Value)
			{
				Data &= ~Mask;
			}
		}

		FORCEINLINE operator bool() const
		{
			return (Data & Mask) != 0;
		}

	private:
		uint64_t& Data;
		uint64_t Mask;
	};

	class FConstBitReference
	{
	public:
		FORCEINLINE explicit FConstBitReference(const uint64_t& Data, const uint64_t Mask)
			: Data(Data)
			, Mask(Mask)
		{
		}

		FORCEINLINE operator bool() const
		{
			return (Data & Mask) != 0;
		}

	private:
		const uint64_t& Data;
		const uint64_t Mask;
	};

	template<IsAllocator InAllocatorType = TAllocator<uint64_t>>
	class TBitArray
	{
	public:
		using AllocatorType = InAllocatorType;

		FORCEINLINE TBitArray() noexcept = default;

		FORCEINLINE TBitArray(const TBitArray& Other)
			: Allocator(Other.Allocator)
			, Size(Other.Size)
			, Capacity(Other.Capacity)
		{
			Data = Allocate(CalculateWordCount(Capacity));
			ConstructElements<uint64_t>(Data, Other.Data, CalculateWordCount(Size));
		}

		FORCEINLINE TBitArray(TBitArray&& Other) noexcept
			: Allocator(std::move(Other.Allocator))
			, Data(std::exchange(Other.Data, nullptr))
			, Size(std::exchange(Other.Size, 0))
			, Capacity(std::exchange(Other.Capacity, 0))
		{
		}

		FORCEINLINE TBitArray(const AllocatorType& Allocator)
			: Allocator(Allocator)
		{
		}

		FORCEINLINE TBitArray(std::initializer_list<uint64_t> InitiailizerList)
		{
			Size = InitiailizerList.size();
			Capacity = Size;

			Data = Allocate(Capacity);
			ConstructElements<uint64_t>(Data, InitiailizerList.begin(), Size);
		}

		TBitArray& operator=(const TBitArray& Other)
		{
			Deallocate(Data, Capacity);

			Allocator = Other.Allocator;
			Size = Other.Size;
			Capacity = Other.Capacity;

			Data = Allocate(Capacity);
			ConstructElements<uint64_t>(Data, Other.Data, Size);

			return *this;
		}

		FORCEINLINE TBitArray& operator=(TBitArray&& Other) noexcept
		{
			Allocator = std::move(Other.Allocator);
			Data = std::exchange(Other.Data, nullptr);
			Size = std::exchange(Other.Size, 0);
			Capacity = std::exchange(Other.Capacity, 0);

			return *this;
		}

		TBitArray& operator=(std::initializer_list<uint64_t> InitiailizerList)
		{
			const size_t NewSize = InitiailizerList.size();

			if (NewSize > Capacity)
			{
				Deallocate(Data, Capacity);

				Size = NewSize;
				Capacity = Size;

				Allocate(Capacity);
			}
			else
			{
				Size = NewSize;
			}

			ConstructElements<uint64_t>(Data, InitiailizerList.begin(), Size);

			return *this;
		}

		NODISCARD FORCEINLINE FBitReference operator[](const size_t Index)
		{
			RangeCheck(Index);
			return GetBitReferenceFromIndex(Index);
		}

		NODISCARD FORCEINLINE FConstBitReference operator[](const size_t Index) const
		{
			RangeCheck(Index);
			return GetBitReferenceFromIndex(Index);
		}

		FORCEINLINE ~TBitArray()
		{
			Dispose();
		}

		NODISCARD FORCEINLINE uint64_t* GetData() noexcept
		{
			return Data;
		}

		NODISCARD FORCEINLINE const uint64_t* GetData() const noexcept
		{
			return Data;
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Size;
		}

		NODISCARD FORCEINLINE size_t GetCapacity() const noexcept
		{
			return Capacity;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return !Size;
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const noexcept
		{
			return Index >= 0 && Index < Size;
		}

		NODISCARD FORCEINLINE FBitReference Last(const size_t IndexFromTheEnd = 0)
		{
			const size_t Index = Size - IndexFromTheEnd - 1;
			RangeCheck(Index);
			return GetBitReferenceFromIndex(Index);
		}

		NODISCARD FORCEINLINE FConstBitReference Last(const size_t IndexFromTheEnd = 0) const
		{
			const size_t Index = Size - IndexFromTheEnd - 1;
			RangeCheck(Index);
			return GetBitReferenceFromIndex(Index);
		}

		FORCEINLINE void Init(const size_t Count, const bool Value = false)
		{
			Clear();
			Add(Value, Count);
		}

		void Reserve(const size_t NewCapacity)
		{
			if (NewCapacity <= Capacity) return;

			uint64_t* OldData = Data;

			Data = Allocate(CalculateWordCount(NewCapacity));
			MoveConstructElements(Data, OldData, CalculateWordCount(Size));

			Deallocate(OldData, CalculateWordCount(Capacity));
			Capacity = NewCapacity;
		}

		void Resize(const size_t NewSize)
		{
			if (NewSize > Size)
			{
				Reserve(NewSize);
				DefaultConstructElements<uint64_t>(Data + Size, CalculateWordCount(NewSize - Size));
			}

			Size = NewSize;
		}

		NODISCARD FORCEINLINE FBitReference Add(const bool Value)
		{
			const size_t Index = AddUninitialized();
			SetBitUnchecked(Index, Value);
			return GetBitReferenceFromIndex(Index);
		}

		NODISCARD FORCEINLINE size_t Add(const bool Value, const size_t BitCountToAdd)
		{
			const size_t Index = AddUninitialized(BitCountToAdd);
			SetBitUnchecked(Index, Value);
			return GetBitReferenceFromIndex(Index);
		}

		FORCEINLINE void RemoveLast() noexcept
		{
			BS_ASSERT(Size, "TBitArray: RemoveLast() called on empty BitArray");
			--Size;
		}

		FORCEINLINE void Shrink()
		{
			uint64_t* OldData = Data;
			const size_t OldCapacity = Capacity;

			Capacity = Size;
			Data = Allocate(CalculateWordCount(Capacity));

			MoveConstructElements(Data, OldData, CalculateWordCount(Size));
			Deallocate(OldData, CalculateWordCount(OldCapacity));
		}

		FORCEINLINE void Clear()
		{
			Size = 0;
		}

		FORCEINLINE void Dispose()
		{
			Clear();
			Deallocate(Data, CalculateWordCount(Capacity));

			Data = nullptr;
			Capacity = 0;
		}

	private:
		const size_t BitsPerWord = CHAR_BIT * sizeof(uint64_t);
		AllocatorType Allocator = AllocatorType();
		uint64_t* Data = nullptr;
		size_t Size = 0;
		size_t Capacity = 0;

		NODISCARD FORCEINLINE size_t GetWordCount() const noexcept
		{
			return Size == 0 ? 0 : (Size - 1) / BitsPerWord;
		}

		NODISCARD FORCEINLINE size_t GetCapacityGrowth() const noexcept
		{
			return (size_t)std::ceil((float)Capacity + (!Capacity ? 1 : Capacity) * 1.5f);
		}

		NODISCARD FORCEINLINE FBitReference GetBitReferenceFromIndex(const size_t Index) noexcept
		{
			return FBitReference(Data[Index / BitsPerWord], 1 << (Index & (BitsPerWord - 1)));
		}

		NODISCARD FORCEINLINE FConstBitReference GetBitReferenceFromIndex(const size_t Index) const noexcept
		{
			return FConstBitReference(Data[Index / BitsPerWord], 1 << (Index & (BitsPerWord - 1)));
		}

		FORCEINLINE void RangeCheck(const size_t Index) const
		{
			BS_ASSERT(Index >= 0 && Index < Size, "TBitArray: index out of bounds, index - {}, size - {}", Index, Size);
		}

		NODISCARD FORCEINLINE size_t CalculateWordCount(const size_t Bits)
		{
			BS_CHECK(Bits >= 0);
			return std::ceil(Bits / BitsPerWord);
		}

		FORCEINLINE void SetWords(const size_t Count, const bool bValue)
		{
			const uint64_t Word = bValue ? 0xff : 0u;

			if (Count > 8)
			{
				memset(Data, Word, Count * sizeof(uint64_t));
			}
			else
			{
				for (size_t Index = 0; Index < Count; ++Index)
				{
					Data[Index] = Word;
				}
			}
		}

		FORCEINLINE void SetBitUnchecked(const size_t Index, const bool Value) const noexcept
		{
			uint64_t& Word = Data[Index / BitsPerWord];
			const uint64_t BitOffset = (Index % BitsPerWord);
			Word = (Word & ~((uint64_t)1 << BitOffset)) | (((uint64_t)Value) << BitOffset);
		}

		FORCEINLINE size_t AddUninitialized(const size_t Count = 1)
		{
			const size_t FirstElementIndex = Size;
			Reserve(Size + Count);
			Size += Count;
			return FirstElementIndex;
		}

		NODISCARD FORCEINLINE uint64_t* Allocate(const size_t Count)
		{
			return ReinterpretCast<uint64_t*>(Allocator.Allocate(Count));
		}

		FORCEINLINE void Deallocate(void* const Ptr, const size_t Count)
		{
			Allocator.Deallocate(Ptr, sizeof(uint64_t) * Count);
		}
	};
}
