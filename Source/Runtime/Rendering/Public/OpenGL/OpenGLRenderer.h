#pragma once

#include "Renderer.h"

namespace Bloodshot
{
	class FOpenGLRenderer final : public IRenderer
	{
		friend class Private::IEngineContext;

	public:
		virtual ~FOpenGLRenderer() override {}

	private:
		FOpenGLRenderer(const glm::vec4& BackgroundColor = {0.2f, 0.3f, 0.3f, 1.0f});

		virtual void Init() override;
		virtual void Dispose() override;

		virtual void DrawTriangles(const TReference<IVertexArray> VertexArray) override;
		virtual void DrawIndexed(const TReference<IVertexArray> VertexArray) override;
		virtual void DrawPart(const TReference<IVertexArray> VertexArray, const FSubMeshInfo& Part) override;
		virtual void DrawLines() override;
		virtual void ClearBackground() override;
	};
}
