#include "OpenGL/OpenGLRenderer.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLHeader.h"
#include "ResourceManager.h"
#include "VertexArray.h"

namespace Bloodshot
{
	FOpenGLRenderer::FOpenGLRenderer(ERendererType Type, const glm::vec4& BackgroundColor)
		: IRenderer(Type, BackgroundColor)
	{
		Instance = this;
	}

	void FOpenGLRenderer::Init()
	{
		BS_LOG(Debug, "Creating OpenGLRenderer...");

		glClearColor((GLfloat)BackgroundColor.r, (GLfloat)BackgroundColor.g, (GLfloat)BackgroundColor.b, (GLfloat)BackgroundColor.a);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		DefaultShader = FResourceManager::LoadShader("DefaultShader", "VertexShader.glsl", "FragmentShader.glsl", true);
	}

	void FOpenGLRenderer::Dispose()
	{
		BS_LOG(Debug, "Destroying OpenGLRenderer...");
	}

	void FOpenGLRenderer::DrawTriangles(const IVertexArray* const VertexArray)
	{
		VertexArray->Bind();

		glDrawArrays(GL_TRIANGLES, 0, VertexArray->GetVertexCount());
	}

	void FOpenGLRenderer::DrawIndexed(const IVertexArray* const VertexArray)
	{
		VertexArray->Bind();

		const IIndexBuffer* const IndexBuffer = VertexArray->GetIndexBuffer();

		IndexBuffer->Bind();

		glDrawElements(GL_TRIANGLES, IndexBuffer->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
	}

	void FOpenGLRenderer::DrawLines()
	{
		// BSTODO: ...
	}

	void FOpenGLRenderer::ClearBackground()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
