#pragma once

#include <cstdint>

namespace Bloodshot
{
	class IIndexBuffer abstract
	{
	public:
		virtual ~IIndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetIndexCount() const = 0;
	};
}
