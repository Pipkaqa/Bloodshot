#include "TempLinearAllocator.h"

namespace Bloodshot
{
	FTempLinearAllocator::FTempLinearAllocator(const size_t Size)
		: Ptr(::operator new(Size))
		, TotalSize(Size)
	{
	}

	FTempLinearAllocator::~FTempLinearAllocator()
	{
		Dispose();
	}

	void* FTempLinearAllocator::Allocate(const size_t Size)
	{
		BS_LOG_IF(Size > TotalSize, Fatal, "Attempting to allocate Size bigger than Size in TempLinearAllocator");

		if (Offset + Size > TotalSize)
		{
			Offset = 0;
		}

		void* const Result = ReinterpretCast<std::byte*>(Ptr) + Offset;
		Offset += Size;
		return Result;
	}

	void FTempLinearAllocator::Reset()
	{
		Offset = 0;
	}

	void FTempLinearAllocator::Dispose()
	{
		::operator delete(Ptr, TotalSize);
	}
}
