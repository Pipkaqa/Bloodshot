#pragma once

#include <cstdint>

namespace Bloodshot
{
	class IUniformBuffer
	{
	public:
		virtual ~IUniformBuffer() {}

		virtual void SetData(const void* Data, const uint32_t Size, uint32_t Offset = 0) = 0;
	};
}
