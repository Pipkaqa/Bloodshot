#include "Malloc.h"
#include "AssertionMacros.h"
#include "Memory/Memory.h"

// BSTODO: Move both functions into FMemory class

void* Malloc(const size_t Size)
{
	void* const Memory = malloc(Size);

	BS_CHECK(Memory, "Failed to allocate memory");

	::Bloodshot::IMemory::OnMemoryAllocated(Size);

	return Memory;
}

void Free(void* const Block, const size_t Size)
{
	if (Block)
	{
		free(Block);
		::Bloodshot::IMemory::OnMemoryDeallocated(Size);
	}
}
