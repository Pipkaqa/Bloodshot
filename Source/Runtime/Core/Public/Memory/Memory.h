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

		NODISCARD FORCEINLINE FAllocationInfo GetAllocationsInfo() const noexcept
		{
			return AllocationsInfo;
		}

		NODISCARD FORCEINLINE bool IsMemoryLeak() const noexcept
		{
			return (AllocationsInfo.AllocatedSize != AllocationsInfo.DeallocatedSize)
				|| (AllocationsInfo.AllocatedBlockCount != AllocationsInfo.DeallocatedBlockCount);
		}

	private:
		FAllocationInfo AllocationsInfo;
		FCurrentMemoryUsageInfo CurrentMemoryUsageInfo;

		FORCEINLINE void OnMemoryAllocated(const size_t Size)
		{
			AllocationsInfo.AllocatedSize += Size;
			++AllocationsInfo.AllocatedBlockCount;

			CurrentMemoryUsageInfo.Size += Size;
			++CurrentMemoryUsageInfo.BlockCount;
		}

		FORCEINLINE void OnMemoryDeallocated(const size_t Size)
		{
			AllocationsInfo.DeallocatedSize += Size;
			++AllocationsInfo.DeallocatedBlockCount;

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
		static void Free(void* const Block, const size_t Size);

		NODISCARD static void* Allocate(const size_t Size, const EAllocationType AllocationType = EAllocationType::Dynamic);
		static void Deallocate(void* const Block, const size_t Size);
	};
}
