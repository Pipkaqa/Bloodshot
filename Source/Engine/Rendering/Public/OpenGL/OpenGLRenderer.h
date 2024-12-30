#pragma once

#include "Renderer.h"

namespace Bloodshot
{
	class FOpenGLRenderer final : public IRenderer
	{
	public:
		FOpenGLRenderer(const glm::vec4& BackgroundColor = {0.2f, 0.3f, 0.3f, 1.0f});
		virtual ~FOpenGLRenderer() override {}

		virtual void Init() override;
		virtual void Dispose() override;

		virtual void DrawTriangles(const TUniquePtr<IVertexArray>& VertexArray) override;
		virtual void DrawIndexed(const TUniquePtr<IVertexArray>& VertexArray) override;
		virtual void DrawPart(const TUniquePtr<IVertexArray>& VertexArray, const FSubMeshInfo& Part) override;
		virtual void DrawLines() override;
		virtual void ClearBackground() override;
	};
}
