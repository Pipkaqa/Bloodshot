#include "TempLinearAllocator.h"

namespace Bloodshot
{
	FTempLinearAllocator::FTempLinearAllocator(const size_t Size)
		: Ptr(::operator new(Size))
		, Size(Size)
	{
	}

	FTempLinearAllocator::~FTempLinearAllocator()
	{
		Dispose();
	}

	FTempLinearAllocatorStats FTempLinearAllocator::GetStats() const
	{
		FTempLinearAllocatorStats Stats;
		Stats.Size = Size;
		Stats.Offset = Offset;

		return Stats;
	}

	void* FTempLinearAllocator::Allocate(const size_t Size)
	{
		BS_LOG_IF(Size > this->Size, Fatal, "Attempting to allocate Size bigger than Size in TempLinearAllocator");

		if (Offset + Size > this->Size)
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
		::operator delete(Ptr, Size);
	}
}
