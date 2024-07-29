#include "OpenGLRenderer.h"

#include "Debug/Logger.h"
#include "Platform/OpenGL/OpenGLHeader.h"


void Bloodshot::OpenGLRenderer::Init()
{
	BS_DEBUG("Creating OpenGL renderer...");

	const auto color = m_Config.m_BackgroundColor;

	glClearColor(color.r, color.g, color.b, color.a);
}

void Bloodshot::OpenGLRenderer::Dispose()
{
	BS_DEBUG("Destroying OpenGL renderer...");
}

void Bloodshot::OpenGLRenderer::DrawTriangles()
{
	// BSTODO: Rename to DrawIndexed, need pass indexes and verticies, and call DrawElements, write another functional too;
}

void Bloodshot::OpenGLRenderer::DrawLines()
{
	// BSTODO: etc... (see up)
}

void Bloodshot::OpenGLRenderer::ClearBackground()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
