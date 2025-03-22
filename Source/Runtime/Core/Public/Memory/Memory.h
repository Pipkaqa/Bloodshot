#pragma once

#include "Platform/Platform.h"

#include <cstdint>

namespace Bloodshot
{
	struct FAllocationInfo final
	{
		size_t AllocatedSize = 0;
		size_t AllocatedBlockCount = 0;
		size_t DeallocatedSize = 0;
		size_t DeallocatedBlockCount = 0;
	};

	struct FCurrentMemoryUsageInfo final
	{
		size_t Size = 0;
		size_t BlockCount = 0;
	};

	class FAllocationLogger final
	{
		friend class FMemory;

	public:
		NODISCARD static FAllocationLogger& GetInstance();

		NODISCARD FORCEINLINE static const FAllocationInfo& GetAllocationsInfo() noexcept
		{
			return GetInstance().AllocationInfo;
		}

		NODISCARD FORCEINLINE static bool IsMemoryLeak() noexcept
		{
			const FAllocationInfo& AllocationInfo = GetAllocationsInfo();
			return (AllocationInfo.AllocatedSize != AllocationInfo.DeallocatedSize)
				|| (AllocationInfo.AllocatedBlockCount != AllocationInfo.DeallocatedBlockCount);
		}

	private:
		FAllocationInfo AllocationInfo;
		FCurrentMemoryUsageInfo CurrentMemoryUsageInfo;

		FORCEINLINE void OnMemoryAllocated(const size_t Size)
		{
			AllocationInfo.AllocatedSize += Size;
			++AllocationInfo.AllocatedBlockCount;

			CurrentMemoryUsageInfo.Size += Size;
			++CurrentMemoryUsageInfo.BlockCount;
		}

		FORCEINLINE void OnMemoryDeallocated(const size_t Size)
		{
			AllocationInfo.DeallocatedSize += Size;
			++AllocationInfo.DeallocatedBlockCount;

			CurrentMemoryUsageInfo.Size -= Size;
			--CurrentMemoryUsageInfo.BlockCount;
		}
	};

	enum class EAllocationType : uint8_t
	{
		Dynamic = 0,
		Temporary
	};

	class FMemory final
	{
	public:
		NODISCARD static FMemory& GetInstance();

		NODISCARD static void* Malloc(const size_t Size);
		static void Free(void* const Block) noexcept;
		NODISCARD static void* Realloc(void* const Block, const size_t Size);

		NODISCARD static void* Allocate(const size_t Size, const EAllocationType AllocationType = EAllocationType::Dynamic);
		static void Deallocate(void* const Block);
	};
}
