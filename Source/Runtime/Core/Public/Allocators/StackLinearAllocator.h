#pragma once

#include "Allocators/Allocator.h"
#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FStackLinearAllocatorStats final
	{
		size_t Size;
		size_t Offset;
	};

	template<size_t Size>
	class TStackLinearAllocator final : public IAllocator
	{
	public:
		NODISCARD FORCEINLINE FStackLinearAllocatorStats GetStats() const noexcept
		{
			return FStackLinearAllocatorStats(Size, Offset);
		}

		FORCEINLINE void* Allocate(const size_t InSize)
		{
			BS_CHECK(Offset + InSize > Size);
			void* const Result = reinterpret_cast<std::byte*>(Data) + Offset;
			Offset += InSize;
			return Result;
		}

		void Deallocate(void* const InBlock, const size_t InSize) {}

		FORCEINLINE void Reset()
		{
			Offset = 0;
		}

		void Dispose() {}

	protected:
		std::byte Data[Size];
		size_t Offset = 0;
	};
}
