#include "Memory/Memory.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Casts.h"

namespace Bloodshot
{
	namespace Private
	{
		struct FAllocationHeader final
		{
			EAllocationType AllocationType;
		};
	}

	FAllocationLogger& FAllocationLogger::GetInstance()
	{
		static FAllocationLogger Instance;
		return Instance;
	}

	FMemory& FMemory::GetInstance()
	{
		static FMemory Instance;
		return Instance;
	}

	void* FMemory::Malloc(const size_t Size)
	{
		auto& a = FAllocationLogger::GetInstance();

		void* const Memory = malloc(Size);

		BS_ASSERT(Memory, "FMemory::Malloc: Failed to allocate memory");

		a.OnMemoryAllocated(Size);

		return Memory;
	}

	void FMemory::Free(void* const Block, const size_t Size)
	{
		if (Block)
		{
			free(Block);
			FAllocationLogger::GetInstance().OnMemoryDeallocated(Size);
		}
	}

	void* FMemory::Allocate(const size_t Size, const EAllocationType AllocationType)
	{
		void* HeaderedBlock = nullptr;

		switch (AllocationType)
		{
			case EAllocationType::Dynamic:
			{
				HeaderedBlock = Malloc(Size + 1);
				break;
			}
			case EAllocationType::Temporary:
			{
				HeaderedBlock = GetInstance().CircularLinearAllocator.Allocate(Size + 1);
				break;
			}
		}

		ReinterpretCast<Private::FAllocationHeader*>(HeaderedBlock)->AllocationType = AllocationType;
		return ReinterpretCast<std::byte*>(HeaderedBlock) + 1;
	}

	void FMemory::Deallocate(void* const Block, const size_t Size)
	{
		void* const HeaderedBlock = ReinterpretCast<std::byte*>(Block) - 1;

		switch (ReinterpretCast<Private::FAllocationHeader*>(HeaderedBlock)->AllocationType)
		{
			case EAllocationType::Dynamic:
			{
				Free(HeaderedBlock, Size + 1);
			}
		}
	}
}
