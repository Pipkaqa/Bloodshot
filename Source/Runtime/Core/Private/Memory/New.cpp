#include "Memory/Memory.h"

void* operator new(const size_t Size)
{
	return ::Bloodshot::FMemory::Malloc(Size);
}

void operator delete(void* const Block) noexcept
{
	::Bloodshot::FMemory::Free(Block);
}
