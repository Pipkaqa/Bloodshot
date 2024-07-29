#pragma once

#include "Rendering/Renderer.h"

namespace Bloodshot
{
	class OpenGLRenderer : public Renderer
	{
		RENDERER_MODULE;

		void Init() override;
		void Dispose() override;

		void DrawTriangles() override;
		void DrawLines() override;
		void ClearBackground() override;
	};
}
