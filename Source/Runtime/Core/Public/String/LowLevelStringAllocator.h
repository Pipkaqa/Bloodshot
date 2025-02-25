#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable 'Data' is uninitialized

#include "Platform/Platform.h"

namespace Bloodshot::Private::String
{
	class FLowLevelStringAllocator final
	{
	public:
		FORCEINLINE char* Allocate(const size_t InSize) noexcept
		{
			char* Result = Data + Offset;
			Offset += InSize;
			return Result;
		}

		FORCEINLINE void Reset() noexcept
		{
			Offset = 0;
		}

	private:
		char Data[16 * 1024];
		size_t Offset = 0;
	};
}

#pragma warning(pop)
