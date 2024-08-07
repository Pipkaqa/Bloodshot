#include "Malloc.h"
#include "Memory/Memory.h"

_VCRT_EXPORT_STD _NODISCARD _Ret_notnull_ _Post_writable_byte_size_(Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(size_t Size)
{
	::Bloodshot::FMemory::OnMemoryAllocatedByNew(Size);
	return Malloc(Size);
}

_VCRT_EXPORT_STD void __CRTDECL operator delete(void* Block, size_t Size) noexcept
{
	::Bloodshot::FMemory::OnMemoryDeallocatedByDelete(Size);
	free(Block);
}
