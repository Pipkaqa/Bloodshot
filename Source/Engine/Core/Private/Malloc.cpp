#include "Malloc.h"

#include "AssertionMacros.h"

void* Malloc(const size_t Size)
{
	void* const Memory = malloc(Size);

	BS_CHECK(Memory, "Failed to allocate memory");

	return Memory;
}
