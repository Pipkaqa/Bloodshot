#pragma once

#include "Core/EngineFramework.h"
#include "Utility/ISingleton.h"

size_t operator""_B(const size_t value);

size_t operator""_KB(const size_t value);

size_t operator""_MB(const size_t value);

size_t operator""_GB(const size_t value);

namespace Bloodshot
{
	class MemoryManager final : public ISingleton<MemoryManager>
	{
		OWNED_BY_CORE;

	public:
		struct Config
		{
			size_t m_LinearMemorySize = 4_MB;

			size_t m_ChunksToPreAllocate = 64;
			size_t m_BlocksPerChunk = 256;

			size_t m_EntitiesStorageGrow = 1024;
			size_t m_ComponentsStorageGrow = 4096;
		};

		FORCEINLINE static const Config& GetConfig()
		{
			return *s_Instance->m_Config;
		}

		FORCEINLINE static size_t GetNewCalls()
		{
			return s_NewCalls;
		}

		FORCEINLINE static size_t GetDeleteCalls()
		{
			return s_DeleteCalls;
		}

		FORCEINLINE static size_t GetTotalMemoryUsage()
		{
			return s_AllocatedBytesByNew + s_AllocatedBytes;
		}

		FORCEINLINE static size_t GetAverageMemorySizeAllocatedByNew()
		{
			return s_AllocatedBytesByNew / s_NewCalls;
		}

		FORCEINLINE static size_t GetCurrentMemorySizeUsage()
		{
			return s_CurrentMemoryInUse + s_AllocatedBytes;
		}

		FORCEINLINE static size_t GetAllocatedBytes()
		{
			return s_AllocatedBytes;
		}

		FORCEINLINE static size_t GetAllocatedBlocks()
		{
			return s_AllocatedBlocks;
		}

		FORCEINLINE static size_t GetReleasedBytes()
		{
			return s_ReleasedBytes;
		}

		FORCEINLINE static size_t GetReleasedBlocks()
		{
			return s_ReleasedBlocks;
		}


	private:
		static size_t s_NewCalls;
		static size_t s_DeleteCalls;
		static size_t s_AllocatedBytesByNew;

		static size_t s_CurrentMemoryInUse;

		static size_t s_AllocatedBytes;
		static size_t s_AllocatedBlocks;

		static size_t s_ReleasedBytes;
		static size_t s_ReleasedBlocks;

		const Config* m_Config;

		NODISCARD static MemoryManager* Create(const Config& config);

		void Init() override {}
		void Dispose() override {}

		template<typename T>
		friend class FixedAllocator;
		friend class LinearAllocator;

		friend void* __cdecl ::operator new(size_t size);
		friend void __cdecl ::operator delete(void* ptr, size_t size);
	};
}

