#pragma once

#include "Allocators/Allocator.h"
#include "Memory/Memory.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Casts.h"
#include "Platform/Platform.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	struct FPoolAllocatorStats final
	{
		size_t BlockHeaderSize;
		size_t BlockSize;
		size_t BlocksPerChunk;
		size_t ChunkSize;
		size_t ChunkCount;
		size_t TotalBlockCount;
		size_t FreeBlockCount;
		size_t BusyBlockCount;
	};

	template<typename InElementType, size_t InBlocksPerChunk>
	class TPoolAllocator final : public IAllocatorBase<InElementType>
	{
	public:
		using Super = IAllocatorBase<InElementType>;
		using ElementType = Super::ElementType;

		template<typename InOtherElementType>
		using Rebind = TPoolAllocator<InOtherElementType, InBlocksPerChunk>;

		FORCEINLINE TPoolAllocator(const size_t InitialChunkCount = 0)
		{
			for (size_t i = 0; i < InitialChunkCount; ++i)
			{
				AllocateChunk();
			}
		}

		virtual ~TPoolAllocator() override
		{
			Dispose();
		}

		NODISCARD FORCEINLINE FPoolAllocatorStats GetStats() const noexcept
		{
			FPoolAllocatorStats Stats =
			{
				.BlockHeaderSize = BlockHeaderSize,
				.BlockSize = BlockSize,
				.BlocksPerChunk = InBlocksPerChunk,
				.ChunkSize = ChunkSize,
				.ChunkCount = ChunksInfo.Count,
				.TotalBlockCount = ChunksInfo.Count * InBlocksPerChunk,
				.FreeBlockCount = FreeBlocksInfo.Count,
				.BusyBlockCount = BusyBlocksInfo.Count
			};

			return Stats;
		}

		NODISCARD virtual void* Allocate(const size_t Count = 1) override
		{
			BS_PROFILE_FUNCTION();
			BS_CHECK(Count);
			if (!FreeBlocksInfo.Count)
			{
				AllocateChunk();
			}

			FBlockHeader* FreeBlock;
			if (FreeBlocksInfo.Count == 1)
			{
				FreeBlock = FreeBlocksInfo.Head;
				FreeBlocksInfo.Head = nullptr;
				FreeBlocksInfo.Tail = nullptr;
			}
			else
			{
				FreeBlock = FreeBlocksInfo.Tail;
				FreeBlocksInfo.Tail = FreeBlocksInfo.Tail->Previous;
				FreeBlocksInfo.Tail->Next = nullptr;
			}
			--FreeBlocksInfo.Count;
			FreeBlock->bInUse = true;
			FreeBlock->Next = nullptr;
			FreeBlock->Previous = nullptr;

			if (!BusyBlocksInfo.Count)
			{
				BusyBlocksInfo.Head = FreeBlock;
				BusyBlocksInfo.Tail = FreeBlock;
			}
			else
			{
				BusyBlocksInfo.Tail->Next = FreeBlock;
				FreeBlock->Previous = BusyBlocksInfo.Tail;
				BusyBlocksInfo.Tail = BusyBlocksInfo.Tail->Next;
			}
			++BusyBlocksInfo.Count;

			ElementType* const FreePlace = FreeBlock->Data;
			return (void*)FreePlace;
		}

		virtual void Deallocate(void* const InBlock, const size_t InSize = 0) override
		{
			BS_PROFILE_FUNCTION();
			BS_CHECK(InBlock);
			BS_CHECK(BusyBlocksInfo.Count);
			FBlockHeader* const Block = (FBlockHeader*)InBlock - 1;
			BS_CHECK(Block->bInUse);
			if (Block != BusyBlocksInfo.Head && Block != BusyBlocksInfo.Tail)
			{
				Block->Previous->Next = Block->Next;
				Block->Next->Previous = Block->Previous;
			}
			else
			{
				if (BusyBlocksInfo.Count == 1)
				{
					BusyBlocksInfo.Head = nullptr;
					BusyBlocksInfo.Tail = nullptr;
				}
				else if (Block == BusyBlocksInfo.Head)
				{
					BusyBlocksInfo.Head = BusyBlocksInfo.Head->Next;
					BusyBlocksInfo.Head->Previous = nullptr;
				}
				else
				{
					BusyBlocksInfo.Tail = BusyBlocksInfo.Tail->Previous;
					BusyBlocksInfo.Tail->Next = nullptr;
				}
			}
			--BusyBlocksInfo.Count;
			Block->Next = nullptr;
			Block->bInUse = false;

			if (!FreeBlocksInfo.Count)
			{
				Block->Previous = nullptr;
				FreeBlocksInfo.Head = Block;
				FreeBlocksInfo.Tail = Block;
			}
			else
			{
				Block->Previous = FreeBlocksInfo.Tail;
				FreeBlocksInfo.Tail->Next = Block;
				FreeBlocksInfo.Tail = Block;
			}
			++FreeBlocksInfo.Count;
		}

		virtual void Reset() override
		{
			BS_PROFILE_FUNCTION();
			if (!BusyBlocksInfo.Count)
			{
				return;
			}

			FBlockHeader* Current = BusyBlocksInfo.Head;
			while (Current)
			{
				Current->bInUse = false;
				Current = Current->Next;
			}

			if (!FreeBlocksInfo.Count)
			{
				FreeBlocksInfo.Head = BusyBlocksInfo.Head;
				FreeBlocksInfo.Tail = FreeBlocksInfo.Head;
				FreeBlocksInfo.Count = BusyBlocksInfo.Count;
			}
			else
			{
				FreeBlocksInfo.Tail->Next = BusyBlocksInfo.Head;
				BusyBlocksInfo.Head->Previous = FreeBlocksInfo.Tail;
				FreeBlocksInfo.Tail = BusyBlocksInfo.Tail;
			}

			BusyBlocksInfo.Head = nullptr;
			BusyBlocksInfo.Tail = nullptr;
			BusyBlocksInfo.Count = 0;
		}

		virtual void Dispose() override
		{
			BS_PROFILE_FUNCTION();
			FChunk* Current = ChunksInfo.Head;
			while (Current)
			{
				FChunk* const ToDelete = Current;
				Current = Current->Next;
				FMemory::Free(ToDelete, ChunkSize);
			}

			IAllocator::OnDeallocate(ChunksInfo.Count * ChunkSize);

			ChunksInfo.Head = nullptr;
			ChunksInfo.Tail = nullptr;
			ChunksInfo.Count = 0;

			FreeBlocksInfo.Head = nullptr;
			FreeBlocksInfo.Tail = nullptr;
			FreeBlocksInfo.Count = 0;

			BusyBlocksInfo.Head = nullptr;
			BusyBlocksInfo.Tail = nullptr;
			BusyBlocksInfo.Count = 0;
		}

	private:
		struct FBlockHeader final
		{
			ElementType* Data;
			FBlockHeader* Next;
			FBlockHeader* Previous;
			bool bInUse;
		};

		template<bool bConst>
		class TBaseIterator
		{
		public:
			using IteratorElementType = std::conditional_t<bConst, const FBlockHeader, FBlockHeader>;

			TBaseIterator(IteratorElementType* const InHead,
				IteratorElementType* const InTail,
				IteratorElementType* const InCurrent)
				: Head(InHead)
				, Tail(InTail)
				, Current(InCurrent)
			{
			}

			FORCEINLINE ElementType& operator*() const
			{
				return *Current->Data;
			}

			FORCEINLINE ElementType* operator->() const
			{
				return Current->Data;
			}

			TBaseIterator& operator++()
			{
				Current = Current->Next;
				return *this;
			}

			TBaseIterator operator++(int)
			{
				TBaseIterator Temp(*this);
				Current = Current->Next;
				return Temp;
			}

			TBaseIterator& operator--()
			{
				Current = Current->Previous;
				return *this;
			}

			TBaseIterator operator--(int)
			{
				TBaseIterator Temp(*this);
				Current = Current->Previous;
				return Temp;
			}

			TBaseIterator& operator+=(const size_t Offset)
			{
				for (size_t i = 0; i < Offset; ++i)
				{
					Current = Current->Next;
				}
				return *this;
			}

			TBaseIterator operator+(const size_t Offset) const
			{
				TBaseIterator Temp(*this);
				return Temp += Offset;
			}

			TBaseIterator& operator-=(const size_t Offset)
			{
				for (size_t i = 0; i < Offset; ++i)
				{
					Current = Current->Previous;
				}
				return *this;
			}

			TBaseIterator operator-(const size_t Offset) const
			{
				TBaseIterator Temp(*this);
				return Temp -= Offset;
			}

			FORCEINLINE explicit operator bool() const noexcept
			{
				return Current && Current->bInUse;
			}

			FORCEINLINE bool operator==(const TBaseIterator& Other) const noexcept
			{
				return Current == Other.Current && Current->bInUse == Other.Current->bInUse;
			}

			FORCEINLINE bool operator!=(const TBaseIterator& Other) const noexcept
			{
				return Current != Other.Current || Current->bInUse != Other.Current->bInUse;
			}

			FORCEINLINE void SetToEnd() noexcept
			{
				Current = Tail;
			}

			FORCEINLINE void Reset() noexcept
			{
				Current = Head;
			}

		private:
			IteratorElementType* const Head;
			IteratorElementType* const Tail;
			IteratorElementType* Current;
		};

	public:
		class FIterator : public TBaseIterator<false>
		{
			using Super = TBaseIterator<false>;

		public:
			FIterator(Super::IteratorElementType* const InHead,
				Super::IteratorElementType* const InTail,
				Super::IteratorElementType* const InCurrent)
				: Super(InHead, InTail, InCurrent)
			{
			}
		};

		class FConstIterator : public TBaseIterator<true>
		{
			using Super = TBaseIterator<true>;

		public:
			FConstIterator(Super::IteratorElementType* const InHead,
				Super::IteratorElementType* const InTail,
				Super::IteratorElementType* const InCurrent)
				: Super(InHead, InTail, InCurrent)
			{
			}
		};

	private:
		struct FChunk final
		{
			FBlockHeader* Data;
			FChunk* Next;
		};

		static constexpr size_t BlockHeaderSize = sizeof(FBlockHeader);
		static constexpr size_t BlockSize = BlockHeaderSize + (sizeof(ElementType) > 8 ? sizeof(ElementType) : 8);
		static constexpr size_t ChunkSize = sizeof(FChunk) + BlockSize * InBlocksPerChunk;

		struct FChunksInfo
		{
			FChunk* Head = nullptr;
			FChunk* Tail = nullptr;
			size_t Count = 0;
		} ChunksInfo;

		struct FFreeBlocksInfo
		{
			FBlockHeader* Head = nullptr;
			FBlockHeader* Tail = nullptr;
			size_t Count = 0;
		} FreeBlocksInfo;

		struct FInUseBlocksInfo
		{
			FBlockHeader* Head = nullptr;
			FBlockHeader* Tail = nullptr;
			size_t Count = 0;
		} BusyBlocksInfo;

		void AllocateChunk()
		{
			BS_PROFILE_FUNCTION();
			FChunk* const NewChunk = (FChunk*)FMemory::Malloc(ChunkSize);
			NewChunk->Data = (FBlockHeader*)(NewChunk + 1);
			NewChunk->Next = nullptr;

			if (!ChunksInfo.Count)
			{
				ChunksInfo.Head = NewChunk;
				ChunksInfo.Tail = ChunksInfo.Head;
			}
			else
			{
				ChunksInfo.Tail->Next = NewChunk;
				ChunksInfo.Tail = ChunksInfo.Tail->Next;
			}

			FBlockHeader* const ChunkData = NewChunk->Data;
			FBlockHeader* CurrentBlock = ChunkData;
			CurrentBlock->Previous = nullptr;
			CurrentBlock->bInUse = false;
			CurrentBlock->Data = (ElementType*)(CurrentBlock + 1);
			for (size_t i = 1; i < InBlocksPerChunk; ++i)
			{
				FBlockHeader* const NextCurrent = (FBlockHeader*)((std::byte*)ChunkData + i * BlockSize);
				CurrentBlock->Next = NextCurrent;
				NextCurrent->Previous = CurrentBlock;
				CurrentBlock = NextCurrent;
				NextCurrent->bInUse = false;
				NextCurrent->Data = (ElementType*)(NextCurrent + 1);
			}
			CurrentBlock->Next = nullptr;

			if (!FreeBlocksInfo.Count)
			{
				FreeBlocksInfo.Head = ChunkData;
				FreeBlocksInfo.Tail = CurrentBlock;
			}
			else
			{
				FreeBlocksInfo.Tail->Next = ChunkData;
				ChunkData->Previous = FreeBlocksInfo.Tail;
			}

			++ChunksInfo.Count;
			FreeBlocksInfo.Count += InBlocksPerChunk;
			IAllocator::OnAllocate(ChunkSize);
		}
	};
}
