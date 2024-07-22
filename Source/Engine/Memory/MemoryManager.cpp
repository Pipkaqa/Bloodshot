#include "MemoryManager.h"

#include "Debug/Logger.h"

#include <corecrt_malloc.h>

size_t operator""_B(const size_t value)
{
	return value;
}

size_t operator""_KB(const size_t value)
{
	return value * 1024_B;
}

size_t operator""_MB(const size_t value)
{
	return value * 1024_KB;
}

size_t operator""_GB(const size_t value)
{
	return value * 1024_MB;
}

namespace Bloodshot
{
	size_t MemoryManager::s_NewCalls = 0;
	size_t MemoryManager::s_DeleteCalls = 0;
	size_t MemoryManager::s_AllocatedBytesByNew = 0;
	size_t MemoryManager::s_CurrentMemoryInUse = 0;

	size_t MemoryManager::s_AllocatedBytes = 0;
	size_t MemoryManager::s_AllocatedBlocks = 0;
	size_t MemoryManager::s_ReleasedBytes = 0;
	size_t MemoryManager::s_ReleasedBlocks = 0;

	MemoryManager* MemoryManager::Create(const Config& config)
	{
		ISingleton::Create();

		FL_CORE_DEBUG("Creating memory manager...");

		s_Instance->m_Config = &config;

		return s_Instance;
	}
}

void* __cdecl operator new(size_t size)
{
	::Bloodshot::MemoryManager::s_NewCalls++;
	::Bloodshot::MemoryManager::s_CurrentMemoryInUse += size;
	::Bloodshot::MemoryManager::s_AllocatedBytesByNew += size;
	//TODO: printf("%i - New call!\n", (int)::Bloodshot::MemoryManager::s_NewCalls);
	return malloc(size);
}

void __cdecl operator delete(void* ptr, size_t size)
{
	::Bloodshot::MemoryManager::s_DeleteCalls++;
	::Bloodshot::MemoryManager::s_CurrentMemoryInUse -= size;
	//TODO: printf("%i - Delete call!\n", (int)::Bloodshot::MemoryManager::s_DeleteCalls);
	free(ptr);
}
