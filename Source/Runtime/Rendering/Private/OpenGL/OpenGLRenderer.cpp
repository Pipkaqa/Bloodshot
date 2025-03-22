#include "OpenGL/OpenGLRenderer.h"
#include "IndexBuffer.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLHeader.h"
#include "VertexArray.h"

#include <cstdint>

namespace Bloodshot
{
	FOpenGLRenderer::FOpenGLRenderer(const glm::vec4& BackgroundColor)
		: IRenderer(ERendererType::OpenGL, BackgroundColor)
	{
		Instance = this;
	}

	void FOpenGLRenderer::Init()
	{
		BS_LOG(Debug, "Creating FOpenGLRenderer...");
		glClearColor((GLfloat)BackgroundColor.r, (GLfloat)BackgroundColor.g, (GLfloat)BackgroundColor.b, (GLfloat)BackgroundColor.a);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}

	void FOpenGLRenderer::Dispose()
	{
		BS_LOG(Debug, "Destroying FOpenGLRenderer...");
	}

	void FOpenGLRenderer::DrawTriangles(const TReference<IVertexArray> VertexArray)
	{
		VertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, VertexArray->GetVertexCount());
		VertexArray->Unbind();
	}

	// BSTODO: Maybe need incapsulate DrawTriangles() and DrawIndexed() into Draw() function, which will choose how to draw?

	void FOpenGLRenderer::DrawIndexed(const TReference<IVertexArray> VertexArray)
	{
		VertexArray->Bind();
		const TUniquePtr<IIndexBuffer>& IndexBuffer = VertexArray->GetIndexBuffer();
		IndexBuffer->Bind();
		glDrawElements(GL_TRIANGLES, IndexBuffer->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
		IndexBuffer->Unbind();
		VertexArray->Unbind();
	}

	void FOpenGLRenderer::DrawPart(const TReference<IVertexArray> VertexArray, const FSubMeshInfo& Part)
	{
		VertexArray->Bind();
		const TUniquePtr<IIndexBuffer>& IndexBuffer = VertexArray->GetIndexBuffer();
		IndexBuffer->Bind();

		//glDrawElementsBaseVertex(GL_TRIANGLES,
		//	Part.IndexCount,
		//	GL_UNSIGNED_INT,
		//	(const void*)(sizeof(Part.StartIndex) * Part.StartIndex),
		//	Part.StartVertex);

		IndexBuffer->Unbind();
		VertexArray->Unbind();
	}

	void FOpenGLRenderer::DrawLines()
	{
		// BSTODO: Implement
	}

	void FOpenGLRenderer::ClearBackground()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
