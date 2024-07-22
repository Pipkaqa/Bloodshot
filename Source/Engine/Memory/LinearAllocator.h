#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	class LinearAllocator final
	{
	public:
		LinearAllocator();
		~LinearAllocator();

		NODISCARD void* Allocate(size_t);

		FORCEINLINE void Reset()
		{
			m_Offset = 0;
		}

	private:
		void* m_Begin = nullptr;

		size_t m_Size = 0;
		size_t m_Offset = 0;
	};
}
