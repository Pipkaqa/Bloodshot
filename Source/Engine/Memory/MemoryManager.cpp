#include "MemoryManager.h"

#include "Benchmark/Profiler.h"
#include "Debug/Logger.h"

namespace Bloodshot
{
	MemoryManager::DefaultAllocatorsMemoryInfo MemoryManager::s_DefaultAllocatorsMemoryInfo = {};
	MemoryManager::CustomAllocatorsMemoryInfo MemoryManager::s_CustomAllocatorsMemoryInfo = {};
	MemoryManager::CurrentMemoryUsageInfo MemoryManager::s_CurrentMemoryUsageInfo = {};

	MemoryManager* MemoryManager::Create(const Config& config)
	{
		BS_ASSERT(!s_Instance, "An attempt to create another memory manager");

		s_Instance = new MemoryManager;

		s_Instance->m_Config = config;

		return s_Instance;
	}

	void MemoryManager::Init()
	{
		BS_DEBUG("Creating memory manager...");
	}

	void MemoryManager::Dispose()
	{
		BS_DEBUG("Destroying memory manager...");
	}
}

_VCRT_EXPORT_STD _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(size_t size)
{
	::Bloodshot::MemoryManager::OnMemoryAllocatedByNew(size);
	//BSTODO: printf("%i - New call! Size: %llu\n", (int)::Bloodshot::MemoryManager::GetDefaultAllocatorsMemoryInfo().m_NewCalls, size);
	return ::Bloodshot::Malloc(size);
}

_VCRT_EXPORT_STD void __CRTDECL operator delete(void* block, size_t size) noexcept
{
	::Bloodshot::MemoryManager::OnMemoryDeallocatedByDelete(size);
	//BSTODO: printf("%i - Delete call! Size: %llu\n", (int)::Bloodshot::MemoryManager::GetDefaultAllocatorsMemoryInfo().m_DeleteCalls, size);
	free(block);
}
