#include "OpenGLRenderer.h"

#include "Platform/OpenGL/OpenGLHeader.h"
#include "Debug/Logger.h"

void Bloodshot::OpenGLRenderer::Init()
{
	FL_CORE_DEBUG("Creating OpenGL renderer...");
}

void Bloodshot::OpenGLRenderer::Dispose()
{
	FL_CORE_DEBUG("Destroying OpenGL renderer...");
}

void Bloodshot::OpenGLRenderer::DrawTriangles()
{
}

void Bloodshot::OpenGLRenderer::DrawLines()
{
}

void Bloodshot::OpenGLRenderer::ClearBackground()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
