#pragma once

#include "Allocator.h"
#include "AssertionMacros.h"
#include "Casts.h"
#include "Platform/Platform.h"
#include "Profiling/ProfilingMacros.h"

#include <cstdint>

namespace Bloodshot
{
	struct FTempLinearAllocatorStats final
	{
		size_t Size;
		size_t Offset;
	};

	class FTempLinearAllocator final : public TAllocator<uint8_t>
	{
	public:
		FTempLinearAllocator(const size_t Size);
		~FTempLinearAllocator();

		NODISCARD FTempLinearAllocatorStats GetStats() const;

		virtual void* Allocate(const size_t Size) override;
		virtual void Deallocate(void* const Block, const size_t Size) override {}

		virtual void Reset() override;
		virtual void Dispose() override;

	private:
		void* Ptr;
		size_t Size;
		size_t Offset = 0;
	};
}
