#pragma once

#include "Allocator.h"
#include "AssertionMacros.h"
#include "Casts.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"

#include <list>
#include <set>

namespace Bloodshot
{
	struct FBlockAllocatorStats final
	{
		size_t ChunkCount;
		size_t InUseBlockCount;
		size_t FreeBlockCount;
		size_t BlockCount;
		size_t BlocksPerChunk;
		size_t BlockHeaderSize;
		size_t BlockSize;
		size_t ChunkSize;
	};

	template<typename InElementType>
	class TBlockAllocator final : public TAllocator<InElementType>
	{
	public:
		using Super = TAllocator<InElementType>;
		using ElementType = Super::ElementType;

		using FMemoryList = std::list<void*>;
		using FMemorySet = std::set<void*>;

		struct FBlockHeader abstract final
		{
			bool bInUse = false;
		};

		class FIterator final
		{
		public:
			FIterator(FMemorySet::iterator Begin, FMemorySet::const_iterator End)
				: CurrentBlock(Begin)
				, End(End)
			{
			}

			FORCEINLINE FIterator& operator++()
			{
				++CurrentBlock;
				return *this;
			}

			FORCEINLINE FIterator operator++(int)
			{
				FIterator Temp = *this;
				++CurrentBlock;
				return Temp;
			}

			NODISCARD FORCEINLINE ElementType& operator*() const
			{
				return *ReinterpretCast<ElementType*>(*CurrentBlock);
			}

			NODISCARD FORCEINLINE ElementType* operator->() const
			{
				return ReinterpretCast<ElementType*>(*CurrentBlock);
			}

			NODISCARD FORCEINLINE bool operator==(const FIterator& Other) const
			{
				return CurrentBlock == Other.CurrentBlock;
			}

			NODISCARD FORCEINLINE bool operator!=(const FIterator& Other) const
			{
				return CurrentBlock != Other.CurrentBlock;
			}

		private:
			FMemorySet::iterator CurrentBlock;
			FMemorySet::const_iterator End;
		};

		TBlockAllocator() = default;

		explicit TBlockAllocator(const size_t BlocksPerChunk = 128, const size_t ChunksToPreAllocate = 0)
			: BlocksPerChunk(BlocksPerChunk)
		{
			BS_ASSERT(BlocksPerChunk, "BlocksPerChunk was 0");

			for (size_t i = 0; i < ChunksToPreAllocate; ++i)
			{
				AllocateChunk();
			}
		}

		virtual ~TBlockAllocator() override
		{
			Dispose();
		}

		TBlockAllocator(TBlockAllocator&& Other) noexcept
			: ChunkList(std::move(Other.ChunkList))
			, FreeBlocksList(std::move(Other.FreeBlocksList))
			, InUseBlocksSet(std::move(Other.InUseBlocksSet))
			, BlocksPerChunk(Other.BlocksPerChunk)
		    , BlockHeaderSize(Other.BlockHeaderSize)
		    , BlockSize(Other.BlockSize)
		    , ChunkSize(Other.ChunkSize)
		{
			Other.Dispose();
		}

		TBlockAllocator& operator=(TBlockAllocator&& Other) noexcept
		{
			ChunkList = std::move(Other.ChunkList);
			FreeBlocksList = std::move(Other.FreeBlocksList);
			InUseBlocksSet = std::move(Other.InUseBlocksSet);
			BlocksPerChunk = Other.BlocksPerChunk;
			BlockHeaderSize = Other.BlockHeaderSize;
			BlockSize = Other.BlockSize;
			ChunkSize = Other.ChunkSize;
			Other.Dispose();

			return *this;
		}

		NODISCARD FBlockAllocatorStats GetStats() const
		{
			FBlockAllocatorStats Stats;
			Stats.ChunkCount = ChunkList.size();
			Stats.InUseBlockCount = InUseBlocksSet.size();
			Stats.FreeBlockCount = FreeBlocksList.size();
			Stats.BlockCount = Stats.InUseBlockCount + Stats.FreeBlockCount;
			Stats.BlocksPerChunk = BlocksPerChunk;
			Stats.BlockHeaderSize = BlockHeaderSize;
			Stats.BlockSize = BlockSize;
			Stats.ChunkSize = ChunkSize;

			return Stats;
		}

		virtual void* Allocate(const size_t Count) override
		{
			BS_PROFILE_FUNCTION();

			if (!FreeBlocksList.size()) AllocateChunk();

			void* const HeaderedBlock = FreeBlocksList.front();

			FreeBlocksList.pop_front();

			ReinterpretCast<FBlockHeader*>(HeaderedBlock)->bInUse = true;

			void* const Block = ReinterpretCast<std::byte*>(HeaderedBlock) + BlockHeaderSize;

			InUseBlocksSet.insert(Block);

			return Block;
		}

		virtual void Deallocate(void* const Block, const size_t Size) override
		{
			BS_PROFILE_FUNCTION();

			if (!Block) return;

			BS_ASSERT(InUseBlocksSet.contains(Block), "Unknown block passed in BlockAllocator for deallocation");

			void* const HeaderedBlock = ReinterpretCast<std::byte*>(Block) - BlockHeaderSize;

			ReinterpretCast<FBlockHeader*>(HeaderedBlock)->bInUse = false;

			FreeBlocksList.push_back(HeaderedBlock);

			InUseBlocksSet.erase(Block);
		}

		virtual void Reset() override
		{
			for (void* const Block : InUseBlocksSet)
			{
				FreeBlocksList.push_back(Block);
			}

			InUseBlocksSet.clear();
		}

		virtual void Dispose() override
		{
			const size_t ChunkCount = ChunkList.size();

			for (void* const Chunk : ChunkList)
			{
				::operator delete(Chunk, ChunkSize);
			}

			ChunkList.clear();
			FreeBlocksList.clear();
			InUseBlocksSet.clear();
		}

		NODISCARD inline FIterator Begin() const
		{
			return FIterator(InUseBlocksSet.begin(), InUseBlocksSet.end());
		}

		NODISCARD inline FIterator End() const
		{
			return FIterator(InUseBlocksSet.end(), InUseBlocksSet.end());
		}

	private:
		FMemoryList ChunkList;
		FMemoryList FreeBlocksList;
		FMemorySet InUseBlocksSet;

		size_t BlocksPerChunk;
		size_t BlockHeaderSize = sizeof(FBlockHeader);
		size_t BlockSize = BlockHeaderSize + (sizeof(ElementType) > 8 ? sizeof(ElementType) : 8);
		size_t ChunkSize = BlocksPerChunk * BlockSize;

		void AllocateChunk()
		{
			BS_PROFILE_FUNCTION();

			void* const ChunkBeginPtr = ::operator new(ChunkSize);

			ChunkList.push_back(ChunkBeginPtr);

			void* CurrentBlockPtr = ChunkBeginPtr;

			for (size_t i = 0; i < BlocksPerChunk; ++i)
			{
				ReinterpretCast<FBlockHeader*>(CurrentBlockPtr)->bInUse = false;

				FreeBlocksList.push_back(CurrentBlockPtr);

				CurrentBlockPtr = ReinterpretCast<std::byte*>(CurrentBlockPtr) + BlockSize;
			}
		}
	};
}
