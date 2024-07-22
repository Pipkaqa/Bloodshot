#include "LinearAllocator.h"

#include "Core/Assert.h"
#include "Debug/Logger.h"
#include "MemoryManager.h"

#include <malloc.h>

namespace Bloodshot
{
	LinearAllocator::LinearAllocator()
		//TODO: Maybe do better
		: m_Begin(malloc(MemoryManager::GetConfig().m_LinearMemorySize)), m_Size(MemoryManager::GetConfig().m_LinearMemorySize)
	{
		auto memoryManager = MemoryManager::s_Instance;

		memoryManager->s_AllocatedBytes += m_Size;
		memoryManager->s_AllocatedBlocks += 1;
	}

	LinearAllocator::~LinearAllocator()
	{
		free(m_Begin);

		auto memoryManager = MemoryManager::s_Instance;

		memoryManager->s_ReleasedBytes += m_Size;
		memoryManager->s_ReleasedBlocks += 1;
	}

	void* LinearAllocator::Allocate(size_t size)
	{
		FL_CORE_ASSERT(m_Offset + size <= m_Size, "Linear allocator has run out of memory");

		FL_CORE_ASSERT(size > 0, /*(m_Size % size == 0) &&*/ "Linear allocator got bad size");

		void* ptr = FastCast<char*>(m_Begin) + m_Offset;
		m_Offset += size;
		return ptr;
	}
}
