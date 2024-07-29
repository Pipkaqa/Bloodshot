#pragma once

#include "Core/EngineFramework.h"
#include "Utility/ISingleton.h"

NODISCARD FORCEINLINE constexpr size_t operator""_KB(const size_t size)
{
	return size << 10;
}

NODISCARD FORCEINLINE constexpr size_t operator""_MB(const size_t size)
{
	return size << 20;
}

NODISCARD FORCEINLINE constexpr size_t operator""_GB(const size_t size)
{
	return size << 30;
}

NODISCARD FORCEINLINE constexpr size_t KB(const size_t size)
{
	return size >> 10;
}

NODISCARD FORCEINLINE constexpr size_t MB(const size_t size)
{
	return size >> 20;
}

NODISCARD FORCEINLINE constexpr size_t GB(const size_t size)
{
	return size >> 30;
}

namespace Bloodshot
{
	class MemoryManager final : public ISingleton<MemoryManager>
	{
		CORE_MODULE;

	public:
		struct Config
		{
			size_t m_LinearMemorySize = 4_MB;
			size_t m_ChunksToPreAllocate = 64;
			size_t m_BlocksPerChunk = 256;
		};

		struct DefaultAllocatorsMemoryInfo
		{
			size_t m_NewCalls = 0;
			size_t m_DeleteCalls = 0;
			size_t m_AllocatedBytes = 0;
			size_t m_DeallocatedBytes = 0;
		};

		struct CustomAllocatorsMemoryInfo
		{
			size_t m_AllocatedBytes = 0;
			size_t m_AllocatedBlocks = 0;
			size_t m_DeallocatedBytes = 0;
			size_t m_DeallocatedBlocks = 0;
		};

		struct CurrentMemoryUsageInfo
		{
			size_t m_Bytes = 0;
		};

		NODISCARD FORCEINLINE static const Config& GetConfig()
		{
			return s_Instance->m_Config;
		}

		NODISCARD FORCEINLINE static const DefaultAllocatorsMemoryInfo& GetDefaultAllocatorsMemoryInfo() noexcept
		{
			return s_DefaultAllocatorsMemoryInfo;
		}

		NODISCARD FORCEINLINE static const CustomAllocatorsMemoryInfo& GetCustomAllocatorsMemoryInfo() noexcept
		{
			return s_CustomAllocatorsMemoryInfo;
		}

		NODISCARD FORCEINLINE static const CurrentMemoryUsageInfo& GetCurrentMemoryUsageInfo() noexcept
		{
			return s_CurrentMemoryUsageInfo;
		}

		NODISCARD FORCEINLINE static bool MemoryLeak() noexcept
		{
			return (s_CustomAllocatorsMemoryInfo.m_AllocatedBytes != s_CustomAllocatorsMemoryInfo.m_DeallocatedBytes)
				|| (s_CustomAllocatorsMemoryInfo.m_AllocatedBlocks != s_CustomAllocatorsMemoryInfo.m_DeallocatedBlocks)
				|| (s_DefaultAllocatorsMemoryInfo.m_AllocatedBytes != s_DefaultAllocatorsMemoryInfo.m_DeallocatedBytes)
				|| (s_DefaultAllocatorsMemoryInfo.m_NewCalls != s_DefaultAllocatorsMemoryInfo.m_DeleteCalls);
		}

	private:
		using ISingleton::Create;

		static DefaultAllocatorsMemoryInfo s_DefaultAllocatorsMemoryInfo;
		static CustomAllocatorsMemoryInfo s_CustomAllocatorsMemoryInfo;
		static CurrentMemoryUsageInfo s_CurrentMemoryUsageInfo;

		Config m_Config = {};

		NODISCARD static MemoryManager* Create(const Config& config);

		void Init() override;
		void Dispose() override;

		FORCEINLINE static void OnMemoryAllocatedByNew(const size_t size)
		{
			s_DefaultAllocatorsMemoryInfo.m_NewCalls++;
			s_DefaultAllocatorsMemoryInfo.m_AllocatedBytes += size;
			s_CurrentMemoryUsageInfo.m_Bytes += size;
		}

		FORCEINLINE static void OnMemoryDeallocatedByDelete(const size_t size)
		{
			s_DefaultAllocatorsMemoryInfo.m_DeleteCalls++;
			s_DefaultAllocatorsMemoryInfo.m_DeallocatedBytes += size;
			s_CurrentMemoryUsageInfo.m_Bytes -= size;
		}

		FORCEINLINE static void OnMemoryAllocatedByCustomAllocator(const size_t size, const size_t blocks = 1)
		{
			s_CustomAllocatorsMemoryInfo.m_AllocatedBytes += size;
			s_CustomAllocatorsMemoryInfo.m_AllocatedBlocks += blocks;
			s_CurrentMemoryUsageInfo.m_Bytes += size;
		}

		FORCEINLINE static void OnMemoryDeallocatedByCustomAllocator(const size_t size, const size_t blocks = 1)
		{
			s_CustomAllocatorsMemoryInfo.m_DeallocatedBytes += size;
			s_CustomAllocatorsMemoryInfo.m_DeallocatedBlocks += blocks;
			s_CurrentMemoryUsageInfo.m_Bytes -= size;
		}

		template<typename T>
		friend class FixedAllocator;
		friend class LinearAllocator;

		friend void* __CRTDECL ::operator new(size_t size);
		friend void __CRTDECL ::operator delete(void* block, size_t size);
	};
}

