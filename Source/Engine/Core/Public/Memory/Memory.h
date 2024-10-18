#pragma once

#include "AssertionMacros.h"
#include "Platform.h"

namespace Bloodshot
{
	class IMemory abstract final
	{
		friend void* ::Malloc(const size_t Size);
		friend void ::Free(void* const Block, const size_t Size);

	public:
		struct FAllocationsInfo final
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

		//static inline size_t ChunksToPreAllocateByFixedAllocators = 64U;
		//static inline size_t BlocksPerChunkInFixedAllocators = 256U;

		NODISCARD FORCEINLINE static const FAllocationsInfo& GetAllocationsInfo() noexcept
		{
			return AllocationsInfo;
		}

		NODISCARD FORCEINLINE static bool IsMemoryLeak() noexcept
		{
			return (AllocationsInfo.AllocatedSize != AllocationsInfo.DeallocatedSize)
				|| (AllocationsInfo.AllocatedBlockCount != AllocationsInfo.DeallocatedBlockCount);
		}

	private:
		static inline FAllocationsInfo AllocationsInfo;
		static inline FCurrentMemoryUsageInfo CurrentMemoryUsageInfo;

		FORCEINLINE static void OnMemoryAllocated(const size_t Size)
		{
			AllocationsInfo.AllocatedSize += Size;
			++AllocationsInfo.AllocatedBlockCount;

			CurrentMemoryUsageInfo.Size += Size;
			++CurrentMemoryUsageInfo.BlockCount;
		}

		FORCEINLINE static void OnMemoryDeallocated(const size_t Size)
		{
			AllocationsInfo.DeallocatedSize += Size;
			++AllocationsInfo.DeallocatedBlockCount;

			CurrentMemoryUsageInfo.Size -= Size;
			--CurrentMemoryUsageInfo.BlockCount;
		}
	};
}
