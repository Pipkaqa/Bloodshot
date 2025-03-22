#pragma once

#include "Allocators/Allocator.h"
#include "Containers/Array.h"
#include "Containers/BitArray.h"
#include "Misc/AssertionMacros.h"
#include "Misc/CoreMisc.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FSparseArrayAllocationInfo final
	{
		size_t Index;
		void* Data;
	};

	template<typename InElementType>
	union TSparseArrayElement
	{
		InElementType Element;

		struct
		{
			size_t NextFreeIndex;
			size_t PrevFreeIndex;
		};
	};

	template<typename InElementType,
		IsAllocator InAllocatorType = FDefaultAllocator,
		IsAllocator InBitArrayAllocatorType = FDefaultAllocator>
	class TSparseArray
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;
		using BitArrayAllocatorType = InBitArrayAllocatorType;

	private:
		using MyElementType = TSparseArrayElement<TAlignedBytes<sizeof(ElementType), alignof(ElementType)>>;
		using DataType = TArray<MyElementType, AllocatorType>;
		DataType Data;

		using AllocationFlagsType = TBitArray<BitArrayAllocatorType>;
		AllocationFlagsType AllocationFlags;

	public:
		using ElementAllocatorType = typename DataType::ElementAllocatorType;

		FORCEINLINE TSparseArray() = default;

		FORCEINLINE TSparseArray(const TSparseArray& Other)
			: Data(Other.Data)
			, AllocationFlags(Other.AllocationFlags)
		{
		}

		FORCEINLINE TSparseArray(TSparseArray&& Other)
			: Data(std::move(Other.Data))
			, AllocationFlags(std::move(Other.AllocationFlags))
		{
		}

		FORCEINLINE TSparseArray(const ElementAllocatorType& InAllocator)
			: Data(InAllocator)
		{
		}

		FORCEINLINE TSparseArray(ElementAllocatorType&& InAllocator)
			: Data(std::move(InAllocator))
		{
		}

		FORCEINLINE ~TSparseArray()
		{
			Dispose();
		}

		FORCEINLINE TSparseArray& operator=(const TSparseArray& Other)
		{
			Data = Other.Data;
			AllocationFlags = Other.AllocationFlags;
			return *this;
		}

		FORCEINLINE TSparseArray& operator=(TSparseArray&& Other)
		{
			Data = std::move(Other.Data);
			AllocationFlags = std::move(Other.AllocationFlags);
			return *this;
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index)
		{
			BS_CHECK(Index >= 0 && Index < Data.GetSize() && Index < AllocationFlags.GetSize());
			return *(ElementType*)&Data[Index].Element;
		}

		NODISCARD FORCEINLINE const ElementType& operator[](const size_t Index) const
		{
			BS_CHECK(Index >= 0 && Index < Data.GetSize() && Index < AllocationFlags.GetSize());
			return *(ElementType*)&Data[Index].Element;
		}

		NODISCARD FORCEINLINE ElementAllocatorType& GetAllocator() noexcept
		{
			Data.GetAllocator();
		}

		NODISCARD FORCEINLINE const ElementAllocatorType& GetAllocator() const noexcept
		{
			Data.GetAllocator();
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Data.GetSize() - FreeIndexCount;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return Data.GetSize() == FreeIndexCount;
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const noexcept
		{
			return AllocationFlags.IsValidIndex(Index) && AllocationFlags[Index];
		}

		void Reserve(const size_t NewSize)
		{
			if (NewSize <= Data.GetSize())
			{
				return;
			}
			const size_t ElementsToAdd = NewSize - Data.GetSize();
			const int64_t ElementsIndex = (int64_t)Data.AddUninitialized(ElementsToAdd);
			MyElementType* const RawData = Data.GetData();

			for (int64_t i = NewSize - 1; i >= ElementsIndex; --i)
			{
				if (FreeIndexCount)
				{
					RawData[FirstFreeIndex].PrevFreeIndex = i;
				}
				RawData[i].PrevFreeIndex = -1;
				RawData[i].NextFreeIndex = FreeIndexCount > 0 ? FirstFreeIndex : IReservedValues::NoneIndex;
				FirstFreeIndex = i;
				++FreeIndexCount;
			}

			if (ElementsToAdd == NewSize)
			{
				AllocationFlags.Init(ElementsToAdd, false);
			}
			else
			{
				AllocationFlags.Add(false, ElementsToAdd);
			}
		}

		NODISCARD FSparseArrayAllocationInfo AllocateIndex(const size_t Index)
		{
			BS_CHECK(Index >= 0);
			BS_CHECK(Index < Data.GetSize());
			BS_CHECK(!AllocationFlags[Index]);

			AllocationFlags[Index] = true;

			FSparseArrayAllocationInfo Result{Index, &Data[Index].Element};
			return Result;
		}

		FSparseArrayAllocationInfo AddUninitialized()
		{
			size_t Index;

			if (FreeIndexCount)
			{
				Index = FirstFreeIndex;
				MyElementType* const RawData = Data.GetData();
				MyElementType& Element = RawData[FirstFreeIndex];

				FirstFreeIndex = Element.NextFreeIndex;
				--FreeIndexCount;

				if (FreeIndexCount)
				{
					Element.PrevFreeIndex = IReservedValues::NoneIndex;
				}
			}
			else
			{
				Index = Data.AddUninitialized();
				AllocationFlags.Add(false);
			}

			return AllocateIndex(Index);
		}

		FORCEINLINE size_t Add(const ElementType& Element)
		{
			FSparseArrayAllocationInfo AllocationInfo = AddUninitialized();
			new(AllocationInfo) ElementType(Element);
			return AllocationInfo.Index;
		}

		FORCEINLINE size_t Add(ElementType&& Element)
		{
			FSparseArrayAllocationInfo AllocationInfo = AddUninitialized();
			new(AllocationInfo) ElementType(std::move(Element));
			return AllocationInfo.Index;
		}

		template<typename... ArgTypes>
		size_t EmplaceBack(ArgTypes&&... Args)
		{
			FSparseArrayAllocationInfo AllocationInfo = AddUninitialized();
			new(AllocationInfo) ElementType(std::forward<ArgTypes>(Args)...);
			return AllocationInfo.Index;
		}

		void RemoveAtUninitialized(size_t Index, size_t Count = 1)
		{
			MyElementType* const RawData = Data.GetData();
			for (; Count; --Count)
			{
				BS_CHECK(AllocationFlags[Index]);
				if (FreeIndexCount)
				{
					RawData[FirstFreeIndex].PrevFreeIndex = Index;
				}
				RawData[Index].PrevFreeIndex = IReservedValues::NoneIndex;
				RawData[Index].NextFreeIndex = FreeIndexCount ? FirstFreeIndex : IReservedValues::NoneIndex;
				FirstFreeIndex = Index;
				++FreeIndexCount;
				AllocationFlags[Index] = false;
				++Index;
			}
		}

		void RemoveAt(size_t Index, size_t Count = 1)
		{
			if constexpr (!std::is_trivially_destructible_v<ElementType>)
			{
				MyElementType* const RawData = Data.GetData();
				for (; Count; --Count)
				{
					DestructElement((ElementType*)&RawData[Index++].Element);
				}
			}
			RemoveAtUninitialized(Index, Count);
		}

		FORCEINLINE void RemoveLastUninitialized()
		{
			RemoveAtUninitialized(Data.GetSize() - 1);
		}

		FORCEINLINE void RemoveLast()
		{
			RemoveAt(Data.GetSize() - 1);
		}

		FORCEINLINE void Clear()
		{
			Data.Clear();
			AllocationFlags.Clear();
			FirstFreeIndex = IReservedValues::NoneIndex;
			FreeIndexCount = 0;
		}

		FORCEINLINE void Dispose()
		{
			Data.Dispose();
			AllocationFlags.Dispose();
			FirstFreeIndex = IReservedValues::NoneIndex;
			FreeIndexCount = 0;
		}

	private:
		template<bool bConst>
		class TBaseIterator
		{
		public:
			using FAllocationFlagsIterator = AllocationFlagsType::FConstSetBitIterator;

		protected:
			using ArrayType = std::conditional_t<bConst, const TSparseArray, TSparseArray>;
			using IteratorElementType = std::conditional_t<bConst, const ElementType, ElementType>;

		public:
			FORCEINLINE explicit TBaseIterator(ArrayType& InArray, const FAllocationFlagsIterator& InAllocationFlagsIt)
				: Array(InArray)
				, AllocationFlagsIt(InAllocationFlagsIt)
			{
			}

			FORCEINLINE TBaseIterator(const TBaseIterator& Other) noexcept
				: Array(Other.Array)
				, AllocationFlagsIt(Other.AllocationFlagsIt)
			{
			}

			NODISCARD FORCEINLINE IteratorElementType& operator*() const
			{
				return Array[GetIndex()];
			}

			NODISCARD FORCEINLINE IteratorElementType* operator->() const
			{
				return &Array[GetIndex()];
			}

			FORCEINLINE TBaseIterator operator++() noexcept
			{
				++AllocationFlagsIt;
				return *this;
			}

			FORCEINLINE TBaseIterator operator++(int) noexcept
			{
				TBaseIterator Temp = *this;
				++AllocationFlagsIt;
				return Temp;
			}

			FORCEINLINE bool operator==(const TBaseIterator& Other) const
			{
				return AllocationFlagsIt == Other.AllocationFlagsIt
					&& &Array == &Other.Array;
			}

			FORCEINLINE bool operator!=(const TBaseIterator& Other) const
			{
				return AllocationFlagsIt != Other.AllocationFlagsIt
					|| &Array != &Other.Array;
			}

			FORCEINLINE bool operator !() const
			{
				return !(bool)AllocationFlagsIt;
			}

			FORCEINLINE explicit operator bool() const
			{
				return (bool)AllocationFlagsIt;
			}

			NODISCARD FORCEINLINE size_t GetIndex() const noexcept
			{
				return AllocationFlagsIt.GetIndex();
			}

			FORCEINLINE void SetToEnd() noexcept
			{
				AllocationFlagsIt.SetToEnd();
			}

		protected:
			ArrayType& Array;
			FAllocationFlagsIterator AllocationFlagsIt;
		};

	public:
		class FIterator : public TBaseIterator<false>
		{
		protected:
			using Super = TBaseIterator<false>;
			using ArrayType = Super::ArrayType;
			using FAllocationFlagsIterator = typename Super::FAllocationFlagsIterator;

		public:
			FORCEINLINE FIterator(ArrayType& InArray)
				: Super(InArray, InArray.AllocationFlags.CreateIterator())
			{
			}

			FORCEINLINE FIterator(ArrayType& InArray, const FAllocationFlagsIterator& InAllocationFlagsIt)
				: Super(InArray, InAllocationFlagsIt)
			{
			}

			FORCEINLINE void RemoveCurrent()
			{
				Super::Array.RemoveAt(Super::GetIndex());
			}
		};

		class FConstIterator : public TBaseIterator<true>
		{
		protected:
			using Super = TBaseIterator<true>;
			using ArrayType = Super::ArrayType;
			using FAllocationFlagsIterator = typename Super::FAllocationFlagsIterator;

		public:
			FORCEINLINE FConstIterator(ArrayType& InArray)
				: Super(InArray, InArray.AllocationFlags.CreateConstIterator())
			{
			}

			FORCEINLINE FConstIterator(ArrayType& InArray, const FAllocationFlagsIterator& InAllocationFlagsIt)
				: Super(InArray, InAllocationFlagsIt)
			{
			}
		};

		NODISCARD FORCEINLINE FIterator CreateIterator()
		{
			return FIterator(*this, AllocationFlagsType::FConstSetBitIterator(AllocationFlags));
		}

		NODISCARD FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return FConstIterator(*this, AllocationFlagsType::FConstSetBitIterator(AllocationFlags));
		}

		class FRangeForIterator : public FIterator
		{
			using Super = FIterator;
			using ArrayType = Super::ArrayType;
			using FAllocationFlagsIterator = typename Super::FAllocationFlagsIterator;

		public:
			FORCEINLINE FRangeForIterator(ArrayType& InArray, const FAllocationFlagsIterator& InAllocationFlagsIt)
				: Super(InArray, InAllocationFlagsIt)
				, InitialSize(InArray.GetSize())
			{
			}

			FORCEINLINE bool operator!=(const FRangeForIterator& Other) const
			{
				BS_ASSERT(Super::Array.GetSize() == InitialSize, "TSparseArray has changed during ranged-for iteration");
				return *(Super*)this != *(Super*)&Other;
			}

		private:
			const size_t InitialSize;
		};

		class FRangeForConstIterator : public FConstIterator
		{
			using Super = FConstIterator;
			using ArrayType = Super::ArrayType;
			using FAllocationFlagsIterator = typename Super::FAllocationFlagsIterator;

		public:
			FORCEINLINE FRangeForConstIterator(ArrayType& InArray, const FAllocationFlagsIterator& InAllocationFlagsIt)
				: Super(InArray, InAllocationFlagsIt)
				, InitialSize(InArray.GetSize())
			{
			}

			FORCEINLINE bool operator!=(const FRangeForConstIterator& Other) const
			{
				BS_ASSERT(Super::Array.GetSize() == InitialSize, "TSparseArray has changed during ranged-for iteration");
				return *(Super*)this != *(Super*)&Other;
			}

		private:
			const size_t InitialSize;
		};

		// For internal usage only!
		FORCEINLINE FRangeForIterator begin() { return FRangeForIterator(*this, AllocationFlagsType::FConstSetBitIterator(AllocationFlags)); }
		FORCEINLINE FRangeForIterator end() { return FRangeForIterator(*this, AllocationFlagsType::FConstSetBitIterator(AllocationFlags, AllocationFlags.GetSize())); }
		FORCEINLINE FRangeForConstIterator begin() const { return FRangeForConstIterator(*this, AllocationFlagsType::FConstSetBitIterator(AllocationFlags)); }
		FORCEINLINE FRangeForConstIterator end() const { return FRangeForConstIterator(*this, AllocationFlagsType::FConstSetBitIterator(AllocationFlags, AllocationFlags.GetSize())); }

	private:
		size_t FirstFreeIndex = 0;
		size_t FreeIndexCount = 0;
	};
}

inline void* operator new(size_t, const Bloodshot::FSparseArrayAllocationInfo& AllocationInfo)
{
	return AllocationInfo.Data;
}

inline void operator delete(void*, const Bloodshot::FSparseArrayAllocationInfo& AllocationInfo)
{
}
