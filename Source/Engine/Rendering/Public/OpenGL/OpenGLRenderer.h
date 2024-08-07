#pragma once

#include "Renderer.h"

namespace Bloodshot
{
	class FOpenGLRenderer : public IRenderer
	{
	public:
		FOpenGLRenderer(ERendererType Type = ERendererType::OpenGL, const glm::uvec4& BackgroundColor = {0U, 1U, 0U, 1U});

		virtual void Init() override;
		virtual void Dispose() override;

		virtual void DrawTriangles() override;
		virtual void DrawLines() override;
		virtual void ClearBackground() override;
	};
}
