#include "Memory/Memory.h"

namespace Bloodshot
{
	FMemory::FMemory()
	{
#ifndef NDEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		Instance = this;
	}

	void* FMemory::Malloc(const size_t Size)
	{
		void* const Memory = malloc(Size);

		BS_CHECK(Memory, "Failed to allocate memory");

		IAllocationLogger::OnMemoryAllocated(Size);

		return Memory;
	}

	void FMemory::Free(void* const Block, const size_t Size)
	{
		if (Block)
		{
			free(Block);
			IAllocationLogger::OnMemoryDeallocated(Size);
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
				HeaderedBlock = Instance->TempLinearAllocator.Allocate(Size + 1);
				break;
			}
		}

		ReinterpretCast<FAllocationHeader*>(HeaderedBlock)->AllocationType = AllocationType;
		return ReinterpretCast<std::byte*>(HeaderedBlock) + 1;
	}

	void FMemory::Deallocate(void* const Block, const size_t Size)
	{
		void* const HeaderedBlock = ReinterpretCast<std::byte*>(Block) - 1;

		switch (ReinterpretCast<FAllocationHeader*>(HeaderedBlock)->AllocationType)
		{
			case EAllocationType::Dynamic:
			{
				Free(HeaderedBlock, Size + 1);
			}
		}
	}
}
