#pragma once

#include "Core.h"

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

		void AddVertexBuffer(TUniquePtr<IVertexBuffer>&& VertexBuffer) override;
		void SetIndexBuffer(TUniquePtr<IIndexBuffer>&& IndexBuffer) override;

	private:
		uint32_t UniqueID = 0;
	};
}
