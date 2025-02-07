#pragma once

#include "Allocators/LinearAllocator.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FCircularLinearAllocatorStats : public FLinearAllocatorStats {};

	class FCircularLinearAllocator : public FLinearAllocator
	{
	public:
		FCircularLinearAllocator(const size_t Size);

		NODISCARD FORCEINLINE FCircularLinearAllocatorStats GetStats() const
		{
			return FCircularLinearAllocatorStats{Size, Offset};
		}

		virtual void* Allocate(const size_t Size) override;
	};
}
