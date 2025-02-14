#pragma once

#include "Allocators/LinearAllocator.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	class FCircularLinearAllocator final : public FLinearAllocator
	{
	public:
		FCircularLinearAllocator(const size_t Size);

		virtual void* Allocate(const size_t Size) override;
	};
}
