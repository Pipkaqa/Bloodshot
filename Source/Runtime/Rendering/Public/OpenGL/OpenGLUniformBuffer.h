#pragma once

#include "UniformBuffer.h"

namespace Bloodshot
{
	class FOpenGLUniformBuffer final : public IUniformBuffer
	{
	public:
		FOpenGLUniformBuffer(const uint32_t Size, const uint32_t Binding);
		virtual ~FOpenGLUniformBuffer() override;

		virtual void SetData(const void* Data, const uint32_t Size, const uint32_t Offset = 0) override;

	private:
		uint32_t UniqueID = 0;
	};
}
