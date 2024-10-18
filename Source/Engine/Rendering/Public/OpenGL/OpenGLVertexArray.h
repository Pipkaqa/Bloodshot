#pragma once

#include "VertexArray.h"

namespace Bloodshot
{
	class FOpenGLVertexArray final : public IVertexArray
	{
	public:
		FOpenGLVertexArray();
		virtual ~FOpenGLVertexArray() override;

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(IVertexBuffer* const VertexBuffer) override;
		void SetIndexBuffer(IIndexBuffer* const IndexBuffer) override;

	private:
		uint32_t UniqueID = 0;
	};
}
