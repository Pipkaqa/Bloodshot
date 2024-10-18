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
	struct FBlockAllocatorSettings final
	{
		size_t ChunksToPreAllocate = 0;
		size_t BlocksInChunk = 128;
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

		explicit TBlockAllocator(const FBlockAllocatorSettings Settings)
			: Settings(Settings)
		{
			BS_ASSERT(Settings.BlocksInChunk, "BlocksInChunk was 0 in BlockAllocatorSettings");

			for (size_t i = 0; i < Settings.ChunksToPreAllocate; ++i)
			{
				AllocateChunk();
			}
		}

		virtual ~TBlockAllocator() override
		{
			Dispose();
		}

		const FBlockAllocatorSettings Settings = {};

		const size_t BlockHeaderSize = sizeof(FBlockHeader);
		const size_t BlockSize = BlockHeaderSize + (sizeof(ElementType) > 8 ? sizeof(ElementType) : 8);
		const size_t ChunkSize = Settings.BlocksInChunk * BlockSize;

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

			InUseBlocksSet.erase(HeaderedBlock);
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

		void AllocateChunk()
		{
			BS_PROFILE_FUNCTION();

			void* const ChunkBeginPtr = ::operator new(ChunkSize);

			ChunkList.push_back(ChunkBeginPtr);

			void* CurrentBlockPtr = ChunkBeginPtr;

			for (size_t i = 0; i < Settings.BlocksInChunk; ++i)
			{
				ReinterpretCast<FBlockHeader*>(CurrentBlockPtr)->bInUse = false;

				FreeBlocksList.push_back(CurrentBlockPtr);

				CurrentBlockPtr = ReinterpretCast<std::byte*>(CurrentBlockPtr) + BlockSize;
			}
		}
	};
}
