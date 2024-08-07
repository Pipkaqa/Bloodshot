#pragma once

#include "AssertionMacros.h"
#include "Platform.h"

namespace Bloodshot
{
	class FMemory abstract final
	{
		friend int ::main(int Argc, char** Argv);

		friend void* __CRTDECL ::operator new(size_t Size);
		friend void __CRTDECL ::operator delete(void* Block, size_t Size);

		template<typename T>
		friend class TBlockAllocator;

		template<typename T>
		friend class TTempBlockAllocator;

		template<typename T>
		friend class TFakeTempAllocator;

	public:
		struct FDefaultAllocatorsMemoryInfo
		{
			size_t NewCallCount = 0U;
			size_t DeleteCallCount = 0U;
			size_t AllocatedSizeByNew = 0U;
			size_t DeallocatedSizeByDelete = 0U;
		};

		struct FEngineAllocatorsMemoryInfo
		{
			size_t AllocatedSize = 0U;
			size_t AllocatedBlockCount = 0U;
			size_t DeallocatedSize = 0U;
			size_t DeallocatedBlockCount = 0U;
		};

		struct FCurrentMemoryUsageInfo
		{
			size_t Size = 0U;
		};

		static inline size_t ChunksToPreAllocateByFixedAllocators = 64U;
		static inline size_t BlocksPerChunkInFixedAllocators = 256U;

		NODISCARD FORCEINLINE static bool IsSessionStarted()
		{
			return bSessionStarted;
		}

		NODISCARD FORCEINLINE static const FDefaultAllocatorsMemoryInfo& GetDefaultAllocatorsMemoryInfo() noexcept
		{
			return DefaultAllocatorsMemoryInfo;
		}

		NODISCARD FORCEINLINE static const FEngineAllocatorsMemoryInfo& GetCustomAllocatorsMemoryInfo() noexcept
		{
			return EngineAllocatorsMemoryInfo;
		}

		NODISCARD FORCEINLINE static const FCurrentMemoryUsageInfo& GetCurrentMemoryUsageInfo() noexcept
		{
			return CurrentMemoryUsageInfo;
		}

		NODISCARD FORCEINLINE static bool IsMemoryLeak() noexcept
		{
			return (EngineAllocatorsMemoryInfo.AllocatedSize != EngineAllocatorsMemoryInfo.DeallocatedSize)
				|| (EngineAllocatorsMemoryInfo.AllocatedBlockCount != EngineAllocatorsMemoryInfo.DeallocatedBlockCount)
				|| (DefaultAllocatorsMemoryInfo.AllocatedSizeByNew != DefaultAllocatorsMemoryInfo.DeallocatedSizeByDelete)
				|| (DefaultAllocatorsMemoryInfo.NewCallCount != DefaultAllocatorsMemoryInfo.DeleteCallCount);
		}

	private:
		static inline FDefaultAllocatorsMemoryInfo StaticAllocationsMemoryInfo;
		static inline FDefaultAllocatorsMemoryInfo DefaultAllocatorsMemoryInfo;
		static inline FEngineAllocatorsMemoryInfo EngineAllocatorsMemoryInfo;
		static inline FCurrentMemoryUsageInfo CurrentMemoryUsageInfo;

		static inline bool bSessionStarted = false;

		FORCEINLINE static void BeginSession()
		{
			BS_ASSERT(!bSessionStarted, "Attempting to start already started Memory Logging Session");

			StaticAllocationsMemoryInfo = DefaultAllocatorsMemoryInfo;

			bSessionStarted = true;
		}

		FORCEINLINE static void EndSession()
		{
			BS_ASSERT(bSessionStarted, "Attempting to end not started Memory Logging Session");

			bSessionStarted = false;
		}

		FORCEINLINE static void OnMemoryAllocatedByNew(const size_t Size)
		{
			if (!bSessionStarted) return;

			DefaultAllocatorsMemoryInfo.NewCallCount++;
			DefaultAllocatorsMemoryInfo.AllocatedSizeByNew += Size;
			CurrentMemoryUsageInfo.Size += Size;
		}

		FORCEINLINE static void OnMemoryDeallocatedByDelete(const size_t Size)
		{
			if (!bSessionStarted) return;

			DefaultAllocatorsMemoryInfo.DeleteCallCount++;
			DefaultAllocatorsMemoryInfo.DeallocatedSizeByDelete += Size;
			CurrentMemoryUsageInfo.Size -= Size;
		}

		FORCEINLINE static void OnMemoryAllocatedByEngineAllocator(const size_t Size, const size_t BlockCount = 1U)
		{
			if (!bSessionStarted) return;

			EngineAllocatorsMemoryInfo.AllocatedSize += Size;
			EngineAllocatorsMemoryInfo.AllocatedBlockCount += BlockCount;
			CurrentMemoryUsageInfo.Size += Size;
		}

		FORCEINLINE static void OnMemoryDeallocatedByEngineAllocator(const size_t Size, const size_t BlockCount = 1U)
		{
			if (!bSessionStarted) return;

			EngineAllocatorsMemoryInfo.DeallocatedSize += Size;
			EngineAllocatorsMemoryInfo.DeallocatedBlockCount += BlockCount;
			CurrentMemoryUsageInfo.Size -= Size;
		}
	};
}
