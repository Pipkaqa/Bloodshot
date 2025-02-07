#include "OpenGL/OpenGLUniformBuffer.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FOpenGLUniformBuffer::FOpenGLUniformBuffer(const uint32_t Size, const uint32_t Binding)
	{
		glCreateBuffers(1, &UniqueID);
		glNamedBufferData(UniqueID, Size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, Binding, UniqueID);
	}

	FOpenGLUniformBuffer::~FOpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &UniqueID);
	}

	void FOpenGLUniformBuffer::SetData(const void* Data, const uint32_t Size, const uint32_t Offset)
	{
		glNamedBufferSubData(UniqueID, Offset, Size, Data);
	}
}
