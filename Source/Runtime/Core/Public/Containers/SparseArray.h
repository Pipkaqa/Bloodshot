#pragma once

#include "Allocators/Allocator.h"
#include "Array.h"
#include "BitArray.h"
#include "CoreMisc.h"
#include "Misc/AssertionMacros.h"
#include "Object/Object.h"
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

	template<typename InElementType, typename InAllocatorType>
	class TSparseArray
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;

		FORCEINLINE ~TSparseArray()
		{
			Dispose();
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index)
		{
			BS_CHECK(Index >= 0 && Index < Data.GetSize() && Index < AllocationFlags.GetSize());
			return Data[Index].Element;
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index) const
		{
			BS_CHECK(Index >= 0 && Index < Data.GetSize() && Index < AllocationFlags.GetSize());
			return Data[Index].Element;
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

		NODISCARD FSparseArrayAllocationInfo AllocateIndex(const size_t Index)
		{
			BS_CHECK(Index >= 0);
			BS_CHECK(Index < Data.GetSize());
			BS_CHECK(!AllocationFlags[Index]);

			AllocationFlags[Index] = true;

			FSparseArrayAllocationInfo Result;
			Result.Index = Index;
			Result.Data = &Data[Index];

			return Result;
		}

		FSparseArrayAllocationInfo AddUninitialized()
		{
			size_t Index;

			if (FreeIndexCount)
			{
				Index = FirstFreeIndex;
				MyElementType* RawData = Data.GetData();
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
		using MyElementType = TSparseArrayElement<InElementType>;
		using DataType = TArray<MyElementType, AllocatorType>;
		DataType Data;

		using FBitArrayAllocator = InAllocatorType::template Rebind<TBitArray<>::ElementType>;
		using FAllocationFlags = TBitArray<FBitArrayAllocator>;
		FAllocationFlags AllocationFlags;

	private:
		template<bool bConst>
		class TBaseIterator
		{
		public:
			using FAllocationFlagsIterator = FAllocationFlags::FIterator;

		private:
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

		protected:
			ArrayType& Array;
			FAllocationFlagsIterator AllocationFlagsIt;
		};

	public:
		class FIterator : public TBaseIterator<false>
		{
			using Super = TBaseIterator<false>;

		public:
			FIterator(TSparseArray& InArray)
				: Super(InArray, InArray.AllocationFlags.CreateIterator())
			{
			}

			FIterator(TSparseArray& InArray, const Super::FAllocationFlagsIterator& InAllocationFlagsIt)
				: Super(InArray, InAllocationFlagsIt)
			{
			}

			void RemoveCurrent()
			{
				Super::Array.RemoveAt(Super::GetIndex());
			}
		};

		class FConstIterator : public TBaseIterator<true>
		{
			using Super = TBaseIterator<true>;

		public:
			FConstIterator(TSparseArray& InArray)
				: Super(InArray, InArray.AllocationFlags.CreateConstIterator())
			{
			}

			FConstIterator(TSparseArray& InArray, const Super::FAllocationFlagsIterator& InAllocationFlagsIt)
				: Super(InArray, InAllocationFlagsIt)
			{
			}
		};

		class FRangeForIterator : public FIterator
		{
			using Super = FIterator;

		public:
			FRangeForIterator(TSparseArray& InArray, const Super::FAllocationFlagsIterator& InAllocationFlagsIt)
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

		public:
			FRangeForConstIterator(TSparseArray& InArray, const Super::FAllocationFlagsIterator& InAllocationFlagsIt)
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
		FORCEINLINE FRangeForIterator begin() { return FRangeForIterator(*this, AllocationFlags.CreateIterator()); }
		FORCEINLINE FRangeForIterator end()
		{
			typename FAllocationFlags::FIterator Iterator = AllocationFlags.CreateIterator();
			Iterator.SetToEnd();
			return FRangeForIterator(*this, Iterator);
		}

		FORCEINLINE FRangeForConstIterator begin() const { return FRangeForConstIterator(*this, AllocationFlags.CreateConstIterator()); }
		FORCEINLINE FRangeForConstIterator end() const
		{
			typename FAllocationFlags::FConstIterator ConstIterator = AllocationFlags.CreateConstIterator();
			ConstIterator.SetToEnd();
			return FRangeForConstIterator(*this, ConstIterator);
		}

	private:
		size_t FirstFreeIndex = 0;
		size_t FreeIndexCount = 0;
	};
}

inline void* operator new(size_t Size, const Bloodshot::FSparseArrayAllocationInfo& AllocationInfo)
{
	return AllocationInfo.Data;
}
