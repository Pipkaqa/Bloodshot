#include "Allocators/CircularLinearAllocator.h"
#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	FCircularLinearAllocator::FCircularLinearAllocator(const size_t Size)
		: FLinearAllocator(Size)
	{
	}

	void* FCircularLinearAllocator::Allocate(const size_t Size)
	{
		BS_LOG_IF(Size > this->Size, Fatal, "FCircularLinearAllocator buffer overflow");

		if (Offset + Size > this->Size)
		{
			Reset();
		}

		void* const Result = ReinterpretCast<std::byte*>(Data) + Offset;
		Offset += Size;
		return Result;
	}
}
