#pragma once

#include "Rendering/Renderer.h"

namespace Bloodshot
{
	class OpenGLRenderer : public Renderer
	{
	public:
		void Init() override;
		void Dispose() override;
		void DrawTriangles() override;
		void DrawLines() override; 
		void ClearBackground() override;
	};
}
