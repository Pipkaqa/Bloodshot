#include "Malloc.h"

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(const size_t Size)
{
	return Malloc(Size);
}

void __CRTDECL operator delete(void* Block, const size_t Size) noexcept
{
	Free(Block, Size);
}
