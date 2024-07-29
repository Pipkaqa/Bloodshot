#pragma once

#include "Benchmark/Profiler.h"
#include "MemoryManager.h"

namespace Bloodshot
{
	using MemoryList = std::list<void*>;

	struct BlockHeader
	{
		size_t m_UniqueID = 0;
	};

	namespace
	{
		static constexpr size_t BlockHeaderSize = sizeof(BlockHeader);

		FORCEINLINE static void AddBlockToMemoryList(void* block, MemoryList& freeBlocks)
		{
			static size_t blockUniqueID = 0;
			auto header = FastCast<BlockHeader*>(block);
			header->m_UniqueID = blockUniqueID++;
			freeBlocks.push_back(block);
		};

		FORCEINLINE static void* ShiftForward(void* block, const size_t value)
		{
			return FastCast<uint8_t*>(block) + value;
		}

		FORCEINLINE static void* ShiftBack(void* block, const size_t value)
		{
			return FastCast<uint8_t*>(block) - value;
		}
	}

	template<typename T>
	class FixedAllocator
	{
	public:
		class Iterator
		{
		public:
			Iterator(MemoryList::iterator begin, MemoryList::iterator end)
				: m_CurrentBlock(begin), m_End(end)
			{
			}

			FORCEINLINE Iterator& operator++()
			{
				++m_CurrentBlock;

				return *this;
			}

			FORCEINLINE Iterator& operator++(int)
			{
				Iterator temp = *this;

				m_CurrentBlock++;

				return temp;
			}

			NODISCARD FORCEINLINE T& operator*() const
			{
				return *FastCast<T*>(*m_CurrentBlock);
			}

			NODISCARD FORCEINLINE T* operator->() const
			{
				return FastCast<T*>(*m_CurrentBlock);
			}

			NODISCARD FORCEINLINE bool operator==(const Iterator& other)
			{
				return (m_CurrentBlock == other.m_CurrentBlock);
			}

			NODISCARD FORCEINLINE bool operator!=(const Iterator& other)
			{
				return (m_CurrentBlock != other.m_CurrentBlock);
			}

		private:
			MemoryList::iterator m_CurrentBlock;
			MemoryList::iterator m_End;
		};

		FixedAllocator()
		{
			if (m_ChunksToPreAllocate > 0)
			{
				for (int i = 0; i < m_ChunksToPreAllocate; i++)
				{
					AllocateChunk();
				}
			}
		}

		~FixedAllocator()
		{
			Release();
		}

		NODISCARD void* Allocate()
		{
			BS_PROFILE_FUNCTION();

			if (!m_FreeBlocks.size())
			{
				AllocateChunk();
			}

			auto block = m_FreeBlocks.back();
			m_FreeBlocks.pop_back();

			auto shifted = ShiftForward(block, BlockHeaderSize);
			m_BusyBlocks[FastCast<BlockHeader*>(block)->m_UniqueID] = shifted;
			m_TempBlocks.clear();

			return shifted;
		}

		void Deallocate(void* block)
		{
			BS_PROFILE_FUNCTION();

			auto shifted = ShiftBack(block, BlockHeaderSize);
			auto blockID = FastCast<BlockHeader*>(shifted)->m_UniqueID;

			BS_ASSERT(m_BusyBlocks.find(blockID) != m_BusyBlocks.end(), "Unknown memory block passed");
			BS_ASSERT(m_BusyBlocks[blockID], "Memory block already deallocated");

			m_FreeBlocks.push_back(shifted);
			m_BusyBlocks[blockID] = nullptr;
			m_TempBlocks.clear();
		}

		void Release()
		{
			const size_t chunkCount = m_Chunks.size();

			for (auto& chunk : m_Chunks)
			{
				free(chunk);
			}

			MemoryManager::OnMemoryDeallocatedByCustomAllocator(m_ChunkSize * chunkCount, m_BlocksPerChunk * chunkCount);
		}

		NODISCARD inline Iterator Begin()
		{
			UpdateBusyBlockList();

			return Iterator(m_TempBlocks.begin(), m_TempBlocks.end());
		}

		NODISCARD inline Iterator End()
		{
			UpdateBusyBlockList();

			return Iterator(m_TempBlocks.end(), m_TempBlocks.end());
		}

	private:
		using MemoryMap = std::unordered_map<size_t, void*>;

		size_t m_ChunksToPreAllocate = MemoryManager::GetConfig().m_ChunksToPreAllocate;
		size_t m_BlocksPerChunk = MemoryManager::GetConfig().m_BlocksPerChunk;
		size_t m_BlockSize = BlockHeaderSize + (sizeof(T) > 8 ? sizeof(T) : 8);
		size_t m_ChunkSize = m_BlockSize * m_BlocksPerChunk;

		MemoryList m_Chunks;
		MemoryList m_FreeBlocks;
		MemoryMap m_BusyBlocks;
		MemoryList m_TempBlocks;

		void AllocateChunk()
		{
			BS_PROFILE_FUNCTION();

			auto chunk = Malloc(m_ChunkSize);

			m_Chunks.push_back(chunk);

			auto block = chunk;

			for (int i = 0; i < m_BlocksPerChunk; i++)
			{
				AddBlockToMemoryList(block, m_FreeBlocks);
				block = ShiftForward(block, m_BlockSize);
			}

			MemoryManager::OnMemoryAllocatedByCustomAllocator(m_ChunkSize, m_BlocksPerChunk);
		}

		void UpdateBusyBlockList()
		{
			BS_PROFILE_FUNCTION();

			if (m_TempBlocks.size()) return;

			for (auto& [uniqueID, block] : m_BusyBlocks)
			{
				if (!block) continue;

				m_TempBlocks.push_back(block);
			}
		}
	};
}
