#include "OpenGL/OpenGLVertexBuffer.h" 
#include "OpenGL/OpenGLHeader.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	FOpenGLVertexBuffer::FOpenGLVertexBuffer(const void* const Data, const uint32_t Size, const uint32_t VertexCount)
		: IVertexBuffer(VertexCount)
	{
		BS_PROFILE_FUNCTION();

		glCreateBuffers(1, &UniqueID);
		glBindBuffer(GL_ARRAY_BUFFER, UniqueID);
		glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	FOpenGLVertexBuffer::~FOpenGLVertexBuffer()
	{
		BS_PROFILE_FUNCTION();

		glDeleteBuffers(1, &UniqueID);
	}

	void FOpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, UniqueID);
	}

	void FOpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void FOpenGLVertexBuffer::SetData(const void* Data, const uint32_t Size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, UniqueID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, Size, Data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void FOpenGLVertexBuffer::SetLayout(const FVertexBufferLayout& Layout)
	{
		this->Layout = Layout;
	}
}
