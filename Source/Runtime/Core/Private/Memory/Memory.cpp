#include "Memory/Memory.h"

namespace Bloodshot
{
	namespace Private
	{
		struct FAllocationHeader final
		{
			EAllocationType AllocationType;
		};
	}

	FAllocationInfo IAllocationLogger::GetAllocationsInfo() noexcept
	{
		return AllocationsInfo;
	}

	bool IAllocationLogger::IsMemoryLeak() noexcept
	{
		return (AllocationsInfo.AllocatedSize != AllocationsInfo.DeallocatedSize)
			|| (AllocationsInfo.AllocatedBlockCount != AllocationsInfo.DeallocatedBlockCount);
	}

	void IAllocationLogger::ClearLog() noexcept
	{
		AllocationsInfo = {};
	}

	FMemory::FMemory()
	{
		Instance = this;
	}

	void* FMemory::Malloc(const size_t Size)
	{
		void* const Memory = malloc(Size);
		
		BS_ASSERT(Memory, "Failed to allocate memory");

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
				HeaderedBlock = Instance->CircularLinearAllocator.Allocate(Size + 1);
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
