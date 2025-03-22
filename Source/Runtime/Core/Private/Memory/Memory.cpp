#include "Memory/Memory.h"
#include "Misc/AssertionMacros.h"

namespace Bloodshot
{
	namespace Private::Memory
	{
		class FInlineCircularLinearAllocator
		{
		public:
			NODISCARD FORCEINLINE void* Allocate(const size_t InSize)
			{
				BS_CHECK(InSize <= Size);
				if (Offset + InSize > Size)
				{
					Offset = 0;
				}
				void* const Result = Data + Offset;
				Offset += InSize;
				return Result;
			}

		private:
			std::byte Data[8192];
			size_t Size = 8192;
			size_t Offset = 0;
		} static GTemporaryBuffer;

		struct FAllocationHeader
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
		BS_CHECK(Size > 0);
		void* const Memory = malloc(Size);
		BS_ASSERT(Memory, "FMemory::Malloc: Failed to allocate memory");
		FAllocationLogger::GetInstance().OnMemoryAllocated(Size);
		return Memory;
	}

	void FMemory::Free(void* const Block) noexcept
	{
		if (Block)
		{
			const size_t BlockSize = _msize(Block);
			free(Block);
			FAllocationLogger::GetInstance().OnMemoryDeallocated(BlockSize);
		}
	}

	void* FMemory::Realloc(void* const Block, const size_t Size)
	{
		BS_CHECK(Size > 0);
		size_t OldBlockSize;
		if (Block)
		{
			OldBlockSize = _msize(Block);
		}
		else
		{
			OldBlockSize = 0;
		}
		void* const NewBlock = realloc(Block, Size);
		BS_ASSERT(NewBlock, "FMemory::Realloc: Failed to reallocate memory");
		FAllocationLogger& AllocationLogger = FAllocationLogger::GetInstance();

		if (Size > OldBlockSize)
		{
			AllocationLogger.OnMemoryAllocated(Size - OldBlockSize);
		}
		else if(OldBlockSize > Size)
		{
			AllocationLogger.OnMemoryDeallocated(OldBlockSize - Size);
		}

		return NewBlock;
	}

	void* FMemory::Allocate(const size_t Size, const EAllocationType AllocationType)
	{
		void* HeaderedBlock;
		switch (AllocationType)
		{
			case EAllocationType::Dynamic:
			{
				HeaderedBlock = Malloc(Size + sizeof(Private::Memory::FAllocationHeader));
				break;
			}
			case EAllocationType::Temporary:
			{
				HeaderedBlock = Private::Memory::GTemporaryBuffer.Allocate(Size + sizeof(Private::Memory::FAllocationHeader));
				break;
			}
		}

		reinterpret_cast<Private::Memory::FAllocationHeader*>(HeaderedBlock)->AllocationType = AllocationType;
		return reinterpret_cast<std::byte*>(HeaderedBlock) + sizeof(Private::Memory::FAllocationHeader);
	}

	void FMemory::Deallocate(void* const Block)
	{
		void* const HeaderedBlock = reinterpret_cast<std::byte*>(Block) - sizeof(Private::Memory::FAllocationHeader);
		switch (reinterpret_cast<Private::Memory::FAllocationHeader*>(HeaderedBlock)->AllocationType)
		{
			case EAllocationType::Dynamic:
			{
				Free(HeaderedBlock);
			}
		}
	}
}
