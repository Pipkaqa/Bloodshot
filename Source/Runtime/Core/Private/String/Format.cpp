#include "String/Format.h"
#include "Allocators/Allocator.h"

namespace Bloodshot::Private::String
{
	char* LLFormatAllocationHelper(IAllocator* const InAllocator, const size_t Size)
	{
		return (char*)InAllocator->Allocate(Size);
	}
}
