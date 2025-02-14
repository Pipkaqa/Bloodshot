#include "Allocators/LinearAllocator.h"
#include "Logging/LoggingMacros.h"
#include "Misc/Casts.h"

namespace Bloodshot
{
	FLinearAllocator::FLinearAllocator(const size_t Size)
		: Data(::operator new(Size))
		, Size(Size)
	{
		IAllocator::OnAllocate(Size);
	}

	FLinearAllocator::~FLinearAllocator()
	{
		Dispose();
	}

	void* FLinearAllocator::Allocate(const size_t Size)
	{
		BS_LOG_IF(Offset + Size > this->Size, Fatal, "FLinearAllocator buffer overflow");

		void* const Result = ReinterpretCast<std::byte*>(Data) + Offset;
		Offset += Size;
		return Result;
	}

	void FLinearAllocator::Reset()
	{
		Offset = 0;
	}

	void FLinearAllocator::Dispose()
	{
		::operator delete(Data, Size);
		IAllocator::OnDeallocate(Size);

		Data = nullptr;
		Size = 0;
		Offset = 0;
	}
}
