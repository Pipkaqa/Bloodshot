#include "OpenGL/OpenGLRenderer.h"

#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FOpenGLRenderer::FOpenGLRenderer(ERendererType Type, const glm::uvec4& BackgroundColor)
		: IRenderer(Type, BackgroundColor)
	{
	}

	void FOpenGLRenderer::Init()
	{
		BS_LOG(Debug, "Creating OpenGLRenderer...");

		glClearColor((GLfloat)BackgroundColor.r, (GLfloat)BackgroundColor.g, (GLfloat)BackgroundColor.b, (GLfloat)BackgroundColor.a);
	}

	void FOpenGLRenderer::Dispose()
	{
		BS_LOG(Debug, "Destroying OpenGLRenderer...");
	}

	void FOpenGLRenderer::DrawTriangles()
	{
		// BSTODO: Rename to DrawIndexed, need pass indexes and verticies, and call DrawElements, write another functional too;
	}

	void FOpenGLRenderer::DrawLines()
	{
		// BSTODO: etc... (see up)
	}

	void FOpenGLRenderer::ClearBackground()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
