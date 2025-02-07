#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLHeader.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	FOpenGLIndexBuffer::FOpenGLIndexBuffer(const uint32_t IndexCount, const uint32_t* const Indices)
		: IndexCount(IndexCount)
	{
		BS_PROFILE_FUNCTION();

		glCreateBuffers(1, &UniqueID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, UniqueID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(uint32_t), Indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	FOpenGLIndexBuffer::~FOpenGLIndexBuffer()
	{
		BS_PROFILE_FUNCTION();

		glDeleteBuffers(1, &UniqueID);
	}

	NODISCARD uint32_t FOpenGLIndexBuffer::GetIndexCount() const
	{
		return IndexCount;
	}

	void FOpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, UniqueID);
	}

	void FOpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
