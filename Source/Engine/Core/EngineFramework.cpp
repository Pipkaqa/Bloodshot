#include "EngineFramework.h"

#include "Debug/Logger.h"
#include "ECS/ECS.h"
#include "ResourceManager.h"
#include "Scene/SceneManager.h"

#include <filesystem>

namespace Bloodshot
{
	void* Malloc(const size_t size)
	{
		auto memory = malloc(size);

		BS_CHECK_FATAL(memory, "Failed to allocated memory!");

		return memory;
	}

	void PrintGraphicsStatus()
	{
		BS_INFO("Graphics API: [OpenGL 4.6, 3D]");
	}

	void PrintSceneStatus()
	{
		BS_INFO("Current scene: [{0}]", SceneManager::GetCurrentScene()->GetTypeName());
	}

	void PrintECSStatus()
	{
		BS_INFO("Entities: [{0}]", 0);
		BS_INFO("Components: [{0}]", 0);
		BS_INFO("Systems: [{0}]", 0);

		BS_INFO("Entity memory pools: [{0}]", 0);
		BS_INFO("Component memory pools: [{0}]", 0);
	}

	void PrintMemoryStatus()
	{
		const auto& defaultAllocatorsMemoryInfo = MemoryManager::GetDefaultAllocatorsMemoryInfo();

		const auto newCalls = defaultAllocatorsMemoryInfo.m_NewCalls;
		const auto deleteCalls = defaultAllocatorsMemoryInfo.m_DeleteCalls;

		const auto allocatedBytesByNew = defaultAllocatorsMemoryInfo.m_AllocatedBytes;
		const auto deallocatedBytesByDelete = defaultAllocatorsMemoryInfo.m_DeallocatedBytes;

		const auto& customAllocatorsMemoryInfo = MemoryManager::GetCustomAllocatorsMemoryInfo();

		const auto allocatedBytesByCustomAllocators = customAllocatorsMemoryInfo.m_AllocatedBytes;
		const auto allocatedBlocksByCustomAllocators = customAllocatorsMemoryInfo.m_AllocatedBlocks;

		const auto deallocatedBytesByCustomAllocators = customAllocatorsMemoryInfo.m_DeallocatedBytes;
		const auto deallocatedBlocksByCustomAllocators = customAllocatorsMemoryInfo.m_DeallocatedBlocks;

		const auto totalAllocatedBytes = allocatedBytesByNew + allocatedBytesByCustomAllocators;
		const auto totalDeallocatedBytes = deallocatedBytesByDelete + deallocatedBytesByCustomAllocators;

		BS_INFO("New calls:                                 {0}", newCalls);
		BS_INFO("Delete calls:                              {0}", deleteCalls);

		BS_INFO("Allocated B by [New]:                      {0}", allocatedBytesByNew);
		BS_INFO("Deallocated B by [Delete]:                 {0}", deallocatedBytesByDelete);

		BS_INFO("Allocated B by [Custom Allocators]:        {0}", allocatedBytesByCustomAllocators);
		BS_INFO("Allocated KB by [Custom Allocators]:       {0}", KB(allocatedBytesByCustomAllocators));
		BS_INFO("Allocated MB by [Custom Allocators]:       {0}", MB(allocatedBytesByCustomAllocators));
		BS_INFO("Allocated GB by [Custom Allocators]:       {0}", GB(allocatedBytesByCustomAllocators));
		BS_INFO("Allocated Blocks by [Custom Allocators]:   {0}", allocatedBlocksByCustomAllocators);

		BS_INFO("Deallocated B by [Custom Allocators]:      {0}", deallocatedBytesByCustomAllocators);
		BS_INFO("Deallocated KB by [Custom Allocators]:     {0}", KB(deallocatedBytesByCustomAllocators));
		BS_INFO("Deallocated MB by [Custom Allocators]:     {0}", MB(deallocatedBytesByCustomAllocators));
		BS_INFO("Deallocated GB by [Custom Allocators]:     {0}", GB(deallocatedBytesByCustomAllocators));
		BS_INFO("Deallocated Blocks by [Custom Allocators]: {0}", deallocatedBlocksByCustomAllocators);

		BS_INFO("Total allocated memory:                    {0}.B / {1}.MB", totalAllocatedBytes, MB(totalAllocatedBytes));
		BS_INFO("Total deallocated memory:                  {0}.B / {1}.MB", totalDeallocatedBytes, MB(totalDeallocatedBytes));
	}

	void PrintEngineStatus()
	{
		PrintGraphicsStatus();

		auto currentWorkingDirectory = std::filesystem::current_path().string();

		BS_INFO("Working directory: [{0}]", currentWorkingDirectory.c_str());
		BS_INFO("Log directory: [{0}\\Log]", currentWorkingDirectory.c_str());
		BS_INFO("Resources directory: [{0}\\{1}]", currentWorkingDirectory.c_str(), ResourceManager::GetConfig().m_ResourcesPath);

		PrintSceneStatus();
		PrintECSStatus();
		PrintMemoryStatus();
	}
}