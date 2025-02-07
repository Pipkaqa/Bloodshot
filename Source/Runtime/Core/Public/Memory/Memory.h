#pragma once

#include "Allocators/CircularLinearAllocator.h"
#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"
#include "Templates/Singleton.h"

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

	class IAllocationLogger final
	{
		friend class FMemory;

	public:
		NODISCARD static FAllocationInfo GetAllocationsInfo() noexcept;
		NODISCARD static bool IsMemoryLeak() noexcept;
		static void ClearLog() noexcept;

	private:
		static inline FAllocationInfo AllocationsInfo;
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

	enum class EAllocationType : uint8_t
	{
		Dynamic = 0,
		Temporary
	};

	class FMemory final : public TSingleton<FMemory>
	{
		friend class IEngineContext;

	public:
		NODISCARD static void* Malloc(const size_t Size);
		static void Free(void* const Block, const size_t Size);

		NODISCARD static void* Allocate(const size_t Size, const EAllocationType AllocationType = EAllocationType::Dynamic);
		static void Deallocate(void* const Block, const size_t Size);

	private:
		FMemory();

		FCircularLinearAllocator CircularLinearAllocator{8192};
	};
}
