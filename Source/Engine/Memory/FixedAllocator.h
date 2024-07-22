#pragma once

#include "Core/Assert.h"
#include "MemoryManager.h"

#include <unordered_map>

namespace Bloodshot
{
	template<typename T>
	class FixedAllocator
	{
	public:
		struct BlockHeader
		{
			size_t m_UniqueID = 0;
		};

		using MemoryList = std::list<void*>;

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
			if (!m_FreeBlocks.size())
			{
				AllocateChunk();
			}

			void* block = m_FreeBlocks.back();

			m_FreeBlocks.pop_back();
			m_BusyBlocks[((BlockHeader*)block)->m_UniqueID] = FastCast<char*>(block) + sizeof(BlockHeader);
			m_TempBlocks.clear();

			return FastCast<char*>(block) + sizeof(BlockHeader);
		}

		void Deallocate(void* block)
		{
			void* headeredBlock = FastCast<char*>(block) - sizeof(BlockHeader);

			FL_CORE_ASSERT(m_BusyBlocks.find(((BlockHeader*)headeredBlock)->m_UniqueID) != m_BusyBlocks.end(), "Unknown memory block passed");
			FL_CORE_ASSERT(m_BusyBlocks[((BlockHeader*)headeredBlock)->m_UniqueID], "Memory block already deallocated");

			m_FreeBlocks.push_back(headeredBlock);
			m_BusyBlocks[((BlockHeader*)headeredBlock)->m_UniqueID] = nullptr;
			m_TempBlocks.clear();
		}

		void Release()
		{
			for (auto& chunk : m_Chunks)
			{
				free(chunk);
			}

			m_FreeBlocks.clear();
			m_BusyBlocks.clear();
			m_TempBlocks.clear();

			static auto memoryManager = MemoryManager::s_Instance;

			memoryManager->s_ReleasedBytes += m_ChunkSize * m_Chunks.size();
			memoryManager->s_ReleasedBlocks += m_BlocksPerChunk * m_Chunks.size();

			m_Chunks.clear();
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
		size_t m_BlockSize = sizeof(BlockHeader) + (sizeof(T) > 8 ? sizeof(T) : 8);
		size_t m_ChunkSize = m_BlockSize * m_BlocksPerChunk;

		MemoryList m_Chunks;
		MemoryList m_FreeBlocks;
		MemoryMap m_BusyBlocks;
		MemoryList m_TempBlocks;

		void AllocateChunk()
		{
			auto chunk = malloc(m_ChunkSize);

			m_Chunks.push_back(chunk);

			auto block = chunk;

			static size_t blockUniqueID = 0;

			for (int i = 0; i < m_BlocksPerChunk - 1; i++)
			{
				auto header = FastCast<BlockHeader*>(block);
				header->m_UniqueID = blockUniqueID++;
				m_FreeBlocks.push_back(block);
				block = FastCast<char*>(block) + m_BlockSize;
			}

			auto header = FastCast<BlockHeader*>(block);
			header->m_UniqueID = blockUniqueID++;
			m_FreeBlocks.push_back(block);

			static auto memoryManager = MemoryManager::s_Instance;

			memoryManager->s_AllocatedBytes += m_ChunkSize;
			memoryManager->s_AllocatedBlocks += m_BlocksPerChunk;
		}

		void UpdateBusyBlockList()
		{
			if (!m_TempBlocks.size())
			{
				for (auto& [uniqueID, block] : m_BusyBlocks)
				{
					if (!block) continue;

					m_TempBlocks.push_back(block);
				}
			}
		}
	};
}
