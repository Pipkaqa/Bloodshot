#pragma once

#include "Allocators/Allocator.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Casts.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FStackLinearAllocatorStats final
	{
		size_t Size;
		size_t Offset;
	};

	template<size_t Size>
	class TStackLinearAllocator final : public IAllocatorBase<std::byte>
	{
	public:
		NODISCARD FORCEINLINE FStackLinearAllocatorStats GetStats() const noexcept
		{
			return FStackLinearAllocatorStats(Size, Offset);
		}

		virtual void* Allocate(const size_t Size) override
		{
			BS_CHECK(Offset + Size > this->Size);
			void* const Result = ReinterpretCast<std::byte*>(Data) + Offset;
			Offset += Size;
			return Result;
		}

		virtual void Deallocate(void* const Block, const size_t Size) override {}

		virtual void Reset() override
		{
			Offset = 0;
		}

		virtual void Dispose() override {}

	protected:
		std::byte Data[Size];
		size_t Offset = 0;
	};
}
