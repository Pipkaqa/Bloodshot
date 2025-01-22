#include "Allocators/LinearAllocator.h"
#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	FLinearAllocator::FLinearAllocator(const size_t Size)
		: Ptr(::operator new(Size))
		, Size(Size)
	{
	}

	FLinearAllocator::~FLinearAllocator()
	{
		Dispose();
	}

	void* FLinearAllocator::Allocate(const size_t Size)
	{
		BS_LOG_IF(Offset + Size > this->Size, Fatal, "FLinearAllocator buffer overflow");

		void* const Result = ReinterpretCast<std::byte*>(Ptr) + Offset;
		Offset += Size;
		return Result;
	}

	void FLinearAllocator::Reset()
	{
		Offset = 0;
	}

	void FLinearAllocator::Dispose()
	{
		::operator delete(Ptr, Size);

		Ptr = nullptr;
		Size = 0;
		Offset = 0;
	}
}
