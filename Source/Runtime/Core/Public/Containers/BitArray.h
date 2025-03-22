#pragma once

#include "Allocators/Allocator.h"
#include "Allocators/AllocatorTraits.h"
#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"
#include "Templates/MemoryOperations.h"

#include <bit>

namespace Bloodshot
{
	class FBitReference
	{
		using ElementType = uint64_t;

	public:
		FORCEINLINE explicit FBitReference(ElementType& Data, ElementType Mask)
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
		ElementType& Data;
		ElementType Mask;
	};

	class FConstBitReference
	{
		using ElementType = uint64_t;

	public:
		FORCEINLINE explicit FConstBitReference(const ElementType& Data, const ElementType Mask)
			: Data(Data)
			, Mask(Mask)
		{
		}

		FORCEINLINE operator bool() const
		{
			return (Data & Mask) != 0;
		}

	private:
		const ElementType& Data;
		const ElementType Mask;
	};

	template<IsAllocator InAllocatorType = FDefaultAllocator>
	class TBitArray
	{
	public:
		using ElementType = uint64_t;
		using AllocatorType = typename TAllocatorTraits<InAllocatorType>::template ForElementType<ElementType>;

		FORCEINLINE TBitArray() noexcept = default;

		FORCEINLINE TBitArray(const TBitArray& Other)
			: Size(Other.Size)
			, Capacity(Other.Capacity)
		{
			ResizeAllocation(0, CalculateWordCount(Capacity));
			memcpy(GetData(), Other.GetData(), Size * sizeof(ElementType));
		}

		FORCEINLINE TBitArray(TBitArray&& Other) noexcept
			: Size(std::exchange(Other.Size, 0))
		{
			Allocator.Move(Other.Allocator);
			Capacity = std::exchange(Other.Capacity, Other.Allocator.GetInitialCapacity());
		}

		FORCEINLINE TBitArray(const AllocatorType& InAllocator)
			: Allocator(InAllocator)
		{
		}

		FORCEINLINE TBitArray(AllocatorType&& InAllocator)
			: Allocator(std::move(InAllocator))
		{
		}

		FORCEINLINE TBitArray(std::initializer_list<ElementType> InitList)
			: Size(InitList.size())
			, Capacity(InitList.size())
		{
			ResizeAllocation(0, Size);
			memcpy(GetData(), InitList.begin(), Size * sizeof(ElementType));
		}

		TBitArray& operator=(const TBitArray& Other)
		{
			Size = Other.Size;
			const size_t NewCapacity = Other.Capacity;
			if (NewCapacity > Capacity)
			{
				ResizeAllocation(CalculateWordCount(Capacity), CalculateWordCount(NewCapacity));
				Capacity = NewCapacity;
			}
			memcpy(GetData(), Other.GetData(), CalculateWordCount(Size) * sizeof(ElementType));
			return *this;
		}

		FORCEINLINE TBitArray& operator=(TBitArray&& Other) noexcept
		{
			Allocator.Move(Other.Allocator);
			Size = std::exchange(Other.Size, 0);
			Capacity = std::exchange(Other.Capacity, Other.Allocator.GetInitialCapacity());
			return *this;
		}

		TBitArray& operator=(std::initializer_list<ElementType> InitList)
		{
			Size = InitList.size();
			const size_t NewCapacity = Size;
			if (NewCapacity > Capacity)
			{
				ResizeAllocation(CalculateWordCount(Capacity), CalculateWordCount(NewCapacity));
				Capacity = NewCapacity * BitsPerWord;
			}
			memcpy(GetData(), InitList.begin(), CalculateWordCount(Size) * sizeof(ElementType));
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

		NODISCARD FORCEINLINE ElementType* GetData() noexcept
		{
			return Allocator.GetAllocation();
		}

		NODISCARD FORCEINLINE const ElementType* GetData() const noexcept
		{
			return Allocator.GetAllocation();
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
			if (NewCapacity <= Capacity)
			{
				return;
			}
			const size_t NewWordCount = CalculateWordCount(NewCapacity);
			ElementType* const OldData = ExchangeAllocation(CalculateWordCount(Capacity), NewWordCount);
			memcpy(GetData(), OldData, CalculateWordCount(Size) * sizeof(ElementType));
			ConsumeAllocation(OldData);
			Capacity = NewWordCount * BitsPerWord;
		}

		void Resize(const size_t NewSize, const bool bValue)
		{
			if (NewSize > Size)
			{
				const size_t BitsToAdd = NewSize - Size;
				const size_t Index = AddUninitialized(BitsToAdd);
				SetRange(Index, BitsToAdd, bValue);
			}
			else if(Size > NewSize)
			{
				SetRange(NewSize, Size - NewSize, bValue);
			}
			Size = NewSize;
		}

		FORCEINLINE FBitReference Add(const bool bValue)
		{
			const size_t Index = AddUninitialized();
			SetBitUnchecked(Index, bValue);
			return GetBitReferenceFromIndex(Index);
		}

		FORCEINLINE size_t Add(const bool bValue, const size_t BitsToAdd)
		{
			const size_t Index = AddUninitialized(BitsToAdd);
			SetRange(Index, BitsToAdd, bValue);
			return GetBitReferenceFromIndex(Index);
		}

		FORCEINLINE void RemoveLast() noexcept
		{
			BS_ASSERT(Size, "TBitArray: RemoveLast() called on empty BitArray");
			--Size;
		}

		FORCEINLINE void Shrink()
		{
			const size_t OldWordCount = CalculateWordCount(Capacity);
			const size_t NewWordCount = CalculateWordCount(Size);
			ElementType* const OldData = ExchangeAllocation(OldWordCount, NewWordCount);
			Capacity = NewWordCount * BitsPerWord;
			memcpy(GetData(), OldData, NewWordCount * sizeof(ElementType));
			ConsumeAllocation(OldData);
		}

		FORCEINLINE void Clear()
		{
			Size = 0;
		}

		FORCEINLINE void Dispose()
		{
			Clear();
			Allocator.Dispose();
			Capacity = 0;
		}

	private:
		template<bool bConst>
		class TIteratorBase
		{
		public:
			TIteratorBase(TBitArray& InArray, const size_t StartIndex)
				: Array(InArray)
				, Index(StartIndex)
			{
			}

			FORCEINLINE FBitReference operator*() const
			{
				return Array[Index];
			}

			FORCEINLINE FBitReference* operator->() const
			{
				return &Array[Index];
			}

			TIteratorBase& operator++()
			{
				++Index;
				return *this;
			}

			TIteratorBase operator++(int)
			{
				TIteratorBase Temp(*this);
				++Index;
				return Temp;
			}

			TIteratorBase& operator--()
			{
				--Index;
				return *this;
			}

			TIteratorBase operator--(int)
			{
				TIteratorBase Temp(*this);
				--Index;
				return Temp;
			}

			TIteratorBase& operator+=(const size_t Offset)
			{
				Index += Offset;
				return *this;
			}

			TIteratorBase operator+(const size_t Offset) const
			{
				TIteratorBase Temp(*this);
				return Temp += Offset;
			}

			TIteratorBase& operator-=(const size_t Offset)
			{
				return *this += -Offset;
			}

			TIteratorBase operator-(const size_t Offset) const
			{
				TIteratorBase Temp(*this);
				return Temp -= Offset;
			}

			FORCEINLINE explicit operator bool() const
			{
				return Array.IsValidIndex(Index);
			}

			NODISCARD FORCEINLINE size_t GetIndex() const
			{
				return Index;
			}

			FORCEINLINE void SetToEnd()
			{
				Index = Array.GetSize();
			}

			FORCEINLINE void RemoveCurrent()
			{
				Array.RemoveAt(Index);
				--Index;
			}

			FORCEINLINE void RemoveSwapCurrent()
			{
				Array.RemoveAtSwap(Index);
				--Index;
			}

			FORCEINLINE void Reset()
			{
				Index = 0;
			}

			FORCEINLINE bool operator==(const TIteratorBase& Other) const
			{
				return &Array == &Other.Array && Index == Other.Index;
			}

			FORCEINLINE bool operator!=(const TIteratorBase& Other) const
			{
				return &Array != &Other.Array || Index != Other.Index;
			}

		private:
			TBitArray& Array;
			size_t Index;
		};

	public:
		class FIterator : public TIteratorBase<false>
		{
			using Super = TIteratorBase<false>;

		public:
			FIterator(TBitArray& InArray, const size_t StartIndex = 0)
				: Super(InArray, StartIndex)
			{
			}
		};

		class FConstIterator : public TIteratorBase<true>
		{
			using Super = TIteratorBase<true>;

		public:
			FConstIterator(TBitArray& InArray, const size_t StartIndex = 0)
				: Super(InArray, StartIndex)
			{
			}
		};

		class FConstSetBitIterator
		{
		public:
			explicit FConstSetBitIterator(const TBitArray& InArray)
				: Array(InArray)
				, WordIndex(0)
				, CurrentBitIndex(0)
				, BaseBitIndex(0)
				, Mask(1ull)
				, UnvisitedBitMask(~0ull)
			{
				if (Array.Size)
				{
					FindFirstSetBit();
				}
			}

			explicit FConstSetBitIterator(const TBitArray& InArray, const size_t InStartIndex)
				: Array(InArray)
				, WordIndex(InStartIndex >> 6ull)
				, CurrentBitIndex(InStartIndex)
				, BaseBitIndex(InStartIndex & ~(BitsPerWord - 1ull))
				, Mask(1ull << (InStartIndex & (BitsPerWord - 1ull)))
				, UnvisitedBitMask((~0ull) << (InStartIndex & (BitsPerWord - 1)))
			{
				BS_CHECK(InStartIndex >= 0 && InStartIndex <= InArray.Size);
				if (InStartIndex != Array.Size)
				{
					FindFirstSetBit();
				}
			}

			FORCEINLINE FConstSetBitIterator& operator++()
			{
				UnvisitedBitMask &= ~Mask;
				FindFirstSetBit();
				return *this;
			}

			FORCEINLINE explicit operator bool() const
			{
				return CurrentBitIndex < Array.Size;
			}

			FORCEINLINE bool operator !() const
			{
				return CurrentBitIndex >= Array.Size;
			}

			FORCEINLINE bool operator==(const FConstSetBitIterator& Other) const
			{
				return CurrentBitIndex == Other.CurrentBitIndex && &Array == &Other.Array;
			}

			FORCEINLINE bool operator!=(const FConstSetBitIterator& Other) const
			{
				return CurrentBitIndex != Other.CurrentBitIndex || &Array != &Other.Array;
			}

			NODISCARD FORCEINLINE size_t GetIndex() const
			{
				return CurrentBitIndex;
			}

			FORCEINLINE void SetToEnd() noexcept
			{
				CurrentBitIndex = Array.Size;
			}

		private:
			const TBitArray& Array;
			size_t WordIndex;
			size_t CurrentBitIndex;
			size_t BaseBitIndex;
			ElementType Mask;
			ElementType UnvisitedBitMask;

			void FindFirstSetBit()
			{
				const ElementType* const Data = Array.GetData();
				const size_t Size = Array.Size;
				const size_t LastWordIndex = (Size - 1) / BitsPerWord;

				ElementType RemainingBitMask = Data[WordIndex] & UnvisitedBitMask;
				while (!RemainingBitMask)
				{
					++WordIndex;
					BaseBitIndex += BitsPerWord;
					if (WordIndex > LastWordIndex)
					{
						CurrentBitIndex = Size;
						return;
					}
					RemainingBitMask = Data[WordIndex];
					UnvisitedBitMask = ~0ull;
				}

				const ElementType NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);
				Mask = NewRemainingBitMask ^ RemainingBitMask;
				CurrentBitIndex = BaseBitIndex + BitsPerWord - 1 - std::countl_zero(Mask);
				if (CurrentBitIndex > Size)
				{
					CurrentBitIndex = Size;
				}
			}
		};

		FORCEINLINE FIterator CreateIterator()
		{
			return FIterator(*this);
		}

		FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return FConstIterator(*this);
		}

	private:
		static inline constexpr ElementType FullWordMask = std::numeric_limits<ElementType>::max();
		static inline constexpr size_t BitsPerWord = CHAR_BIT * sizeof(ElementType);
		AllocatorType Allocator;
		size_t Size = 0;
		size_t Capacity = 0;

		NODISCARD FORCEINLINE size_t GetWordCount() const noexcept
		{
			return (size_t)((Size + BitsPerWord - 1) / BitsPerWord);
		}

		NODISCARD FORCEINLINE size_t GetCapacityGrowth() const noexcept
		{
			return (size_t)std::ceil((float)Capacity + (!Capacity ? 1 : Capacity) * 1.5f);
		}

		NODISCARD FORCEINLINE FBitReference GetBitReferenceFromIndex(const size_t Index) noexcept
		{
			return FBitReference(GetData()[Index / BitsPerWord], 1ull << (Index & (BitsPerWord - 1ull)));
		}

		NODISCARD FORCEINLINE FConstBitReference GetBitReferenceFromIndex(const size_t Index) const noexcept
		{
			return FConstBitReference(GetData()[Index / BitsPerWord], 1ull << (Index & (BitsPerWord - 1ull)));
		}

		FORCEINLINE void RangeCheck(const size_t Index) const
		{
			BS_ASSERT(Index < Size, "TBitArray: index out of bounds, index - {}, size - {}", Index, Size);
		}

		NODISCARD FORCEINLINE size_t CalculateWordCount(const size_t Bits)
		{
			return (size_t)((Bits + BitsPerWord - 1) / BitsPerWord);
		}

		FORCEINLINE void SetWords(const size_t InIndex, const size_t Count, const bool bValue)
		{
			ElementType* const Data = GetData();
			const int Word = bValue ? 0xff : 0;
			if (Count > 8)
			{
				memset(Data, Word, Count * sizeof(ElementType));
			}
			else
			{
				for (size_t Index = InIndex; Index < Count; ++Index)
				{
					Data[Index] = Word;
				}
			}
		}

		FORCEINLINE void SetWords(const size_t Count, const bool bValue)
		{
			SetWords(0, Count, bValue);
		}

		FORCEINLINE void SetBitUnchecked(const size_t Index, const bool Value) noexcept
		{
			ElementType& Word = GetData()[Index / BitsPerWord];
			const ElementType BitOffset = (Index % BitsPerWord);
			Word = (Word & ~((ElementType)1 << BitOffset)) | (((ElementType)Value) << BitOffset);
		}

		FORCEINLINE void SetRange(const size_t Index, const size_t BitsToSet, const bool bValue)
		{
			BS_CHECK(Index + BitsToSet <= Size);
			if (!BitsToSet)
			{
				return;
			}

			const size_t StartIndex = Index / BitsPerWord;
			size_t Count = (Index + BitsToSet + (BitsPerWord - 1)) / BitsPerWord - StartIndex;

			const ElementType StartMask = FullWordMask << (Index % BitsPerWord);
			const ElementType EndMask = FullWordMask >> (BitsPerWord - (Index + BitsToSet) % BitsPerWord) % BitsPerWord;
			
			ElementType* Data = GetData() + StartIndex;
			if (bValue)
			{
				if (Count == 1)
				{
					*Data |= StartMask & EndMask;
				}
				else
				{
					*Data++ |= StartMask;
					Count -= 2;
					while (Count)
					{
						*Data++ = ~0ull;
						--Count;
					}
					*Data |= EndMask;
				}
			}
			else
			{
				if (Count == 1)
				{
					*Data &= ~(StartMask & EndMask);
				}
				else
				{
					*Data++ &= ~StartMask;
					Count -= 2;
					while (Count)
					{
						*Data++ = 0;
						--Count;
					}
					*Data &= ~EndMask;
				}
			}
		}

		FORCEINLINE size_t AddUninitialized(const size_t Count = 1)
		{
			const size_t FirstElementIndex = Size;
			Reserve(Size + Count);
			Size += Count;
			return FirstElementIndex;
		}

		NODISCARD FORCEINLINE ElementType* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize)
		{
			return Allocator.ExchangeAllocation(CurrentSize, NewSize, sizeof(ElementType));
		}

		FORCEINLINE void ConsumeAllocation(ElementType* const Data) const noexcept
		{
			Allocator.ConsumeAllocation((void*)Data);
		}

		FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize)
		{
			Allocator.ResizeAllocation(CurrentSize, NewSize, sizeof(ElementType));
		}
	};
}
