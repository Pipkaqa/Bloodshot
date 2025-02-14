#pragma once

#include "Allocators/Allocator.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FLinearAllocatorStats final
	{
		size_t Size;
		size_t Offset;
	};

	class FLinearAllocator : public IAllocatorBase<std::byte>
	{
	public:
		FLinearAllocator(const size_t Size);
		virtual ~FLinearAllocator() override;

		NODISCARD FORCEINLINE FLinearAllocatorStats GetStats() const
		{
			return FLinearAllocatorStats(Size, Offset);
		}

		virtual void* Allocate(const size_t Size) override;
		virtual void Deallocate(void* const Block, const size_t Size) override {}

		virtual void Reset() override;
		virtual void Dispose() override;

	protected:
		void* Data;
		size_t Size;
		size_t Offset = 0;
	};
}
