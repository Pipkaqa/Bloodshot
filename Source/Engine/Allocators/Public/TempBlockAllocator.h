#pragma once

#include "Allocator.h"
#include "AssertionMacros.h"
#include "Casts.h"
#include "Malloc.h"
#include "Memory/Memory.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"

#include <list>
#include <set>

namespace Bloodshot
{
	struct FTempBlockAllocatorSettings final
	{
		size_t ChunkCount = 4;
		size_t BlocksInChunk = 128;
	};

	template<typename InElementType>
	class TTempBlockAllocator : public IAllocator
	{
	public:
		using ElementType = InElementType;

		struct FBlockHeader abstract final
		{
			bool bInUse = false;
		};

		TTempBlockAllocator(const FTempBlockAllocatorSettings Settings)
			: Settings(Settings)
		{
			BS_ASSERT(Settings.ChunkCount, "ChunkCount was 0 in TempBlockAllocatorSettings");
			BS_ASSERT(Settings.BlocksInChunk, "BlocksInChunk was 0 in TempBlockAllocatorSettings");

			for (size_t i = 0U; i < Settings.ChunkCount; i++)
			{
				AllocateChunk();
			}
		}

		virtual ~TTempBlockAllocator() override
		{
			Dispose();
		}

		const FTempBlockAllocatorSettings Settings;

		const size_t BlockHeaderSize = sizeof(FBlockHeader);
		const size_t BlockSize = BlockHeaderSize + (sizeof(ElementType) > 8U ? sizeof(ElementType) : 8U);
		const size_t ChunkSize = Settings.BlocksInChunk * BlockSize;

		virtual void* Allocate() override
		{
			BS_PROFILE_FUNCTION();

			void* const HeaderedBlock = BlocksList.front();

			BlocksList.pop_front();

			BlocksList.push_back(HeaderedBlock);

			ReinterpretCast<FBlockHeader*>(HeaderedBlock)->bInUse = true;

			void* const Block = ReinterpretCast<std::byte*>(HeaderedBlock) + BlockHeaderSize;

			return Block;
		}

		virtual void Deallocate(void* const Block) override
		{
			BS_LOG(Error, "Deallocate() called in TempBlockAllocator");
		}

		virtual void Dispose() override
		{
			const size_t ChunkCount = ChunkList.size();

			for (void* const Chunk : ChunkList)
			{
				free(Chunk);
			}

			ChunkList.clear();
			BlocksList.clear();

			FMemory::OnMemoryDeallocatedByEngineAllocator(ChunkSize * ChunkCount, Settings.BlocksInChunk * ChunkCount);
		}

	private:
		std::list<void*> ChunkList;
		std::list<void*> BlocksList;

		void AllocateChunk()
		{
			BS_PROFILE_FUNCTION();

			void* const ChunkBeginPtr = Malloc(ChunkSize);

			ChunkList.push_back(ChunkBeginPtr);

			void* CurrentBlockPtr = ChunkBeginPtr;

			for (size_t i = 0U; i < Settings.BlocksInChunk; ++i)
			{
				ReinterpretCast<FBlockHeader*>(CurrentBlockPtr)->bInUse = false;

				BlocksList.push_back(CurrentBlockPtr);

				CurrentBlockPtr = ReinterpretCast<std::byte*>(CurrentBlockPtr) + BlockSize;
			}

			FMemory::OnMemoryAllocatedByEngineAllocator(ChunkSize, Settings.BlocksInChunk);
		}
	};
}