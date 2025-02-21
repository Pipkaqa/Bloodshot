#include "Memory/Memory.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Casts.h"

namespace Bloodshot
{
	namespace Private
	{
		class FCircularStackLinearAllocator final
		{
		public:
			NODISCARD FORCEINLINE void* Allocate(const size_t InSize)
			{
				BS_CHECK(InSize <= Size);
				if (Offset + InSize > Size)
				{
					Offset = 0;
				}

				void* Result = Data + Offset;
				Offset += InSize;
				return Data;
			}

		private:
			std::byte Data[8192];
			size_t Size = 8192;
			size_t Offset = 0;
		} static GTemporaryBuffer;

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
		void* const Memory = malloc(Size);
		BS_ASSERT(Memory, "FMemory::Malloc: Failed to allocate memory");
		FAllocationLogger::GetInstance().OnMemoryAllocated(Size);
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
				HeaderedBlock = Malloc(Size + sizeof(Private::FAllocationHeader));
				break;
			}
			case EAllocationType::Temporary:
			{
				HeaderedBlock = Private::GTemporaryBuffer.Allocate(Size + sizeof(Private::FAllocationHeader));
				break;
			}
		}

		ReinterpretCast<Private::FAllocationHeader*>(HeaderedBlock)->AllocationType = AllocationType;
		return ReinterpretCast<std::byte*>(HeaderedBlock) + sizeof(Private::FAllocationHeader);
	}

	void FMemory::Deallocate(void* const Block, const size_t Size)
	{
		void* const HeaderedBlock = ReinterpretCast<std::byte*>(Block) - sizeof(Private::FAllocationHeader);

		switch (ReinterpretCast<Private::FAllocationHeader*>(HeaderedBlock)->AllocationType)
		{
			case EAllocationType::Dynamic:
			{
				Free(HeaderedBlock, Size + sizeof(Private::FAllocationHeader));
			}
		}
	}
}
