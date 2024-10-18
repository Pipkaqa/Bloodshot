#include "OpenGL/OpenGLVertexArray.h"
#include "AssertionMacros.h"
#include "OpenGL/OpenGLHeader.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	FOpenGLVertexArray::FOpenGLVertexArray()
	{
		BS_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &UniqueID);
	}

	FOpenGLVertexArray::~FOpenGLVertexArray()
	{
		BS_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &UniqueID);
	}

	void FOpenGLVertexArray::Bind() const
	{
		BS_PROFILE_FUNCTION();

		glBindVertexArray(UniqueID);
	}

	void FOpenGLVertexArray::Unbind() const
	{
		BS_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void FOpenGLVertexArray::AddVertexBuffer(IVertexBuffer* const VertexBuffer)
	{
		BS_PROFILE_FUNCTION();
		
		const FVertexBufferLayout& VertexBufferLayout = VertexBuffer->GetLayout();

		BS_ASSERT(VertexBufferLayout.GetElements().size(), "Attempting to add VertexBuffer without Layout");

		glBindVertexArray(UniqueID);

		VertexBuffer->Bind();

		using VertexBufferElementConstIt = std::vector<FVertexBufferElement>::const_iterator;

		VertexBufferElementConstIt VertexBufferElementBeginIt = VertexBufferLayout.GetConstBeginIt();
		VertexBufferElementConstIt VertexBufferElementEndIt = VertexBufferLayout.GetConstEndIt();

		for (VertexBufferElementConstIt ElementIt = VertexBufferElementBeginIt; ElementIt < VertexBufferElementEndIt; ++ElementIt)
		{
			EShaderDataType ShaderDataType = ElementIt->Type;

			switch (ShaderDataType)
			{
				case EShaderDataType::Bool: FALLTHROUGH;
				case EShaderDataType::Int: FALLTHROUGH;
				case EShaderDataType::Int2: FALLTHROUGH;
				case EShaderDataType::Int3: FALLTHROUGH;
				case EShaderDataType::Int4:
				{
					glEnableVertexAttribArray(VertexBufferIndex);

					glVertexAttribIPointer(VertexBufferIndex,
						GetShaderDataTypeElementCount(ShaderDataType),
						ShaderDataTypeToOpenGLBaseType(ShaderDataType),
						VertexBufferLayout.GetStride(),
						(const void*)ElementIt->Offset);

					++VertexBufferIndex;

					break;
				}
				case EShaderDataType::Float: FALLTHROUGH;
				case EShaderDataType::Float2: FALLTHROUGH;
				case EShaderDataType::Float3: FALLTHROUGH;
				case EShaderDataType::Float4:
				{
					glEnableVertexAttribArray(VertexBufferIndex);

					glVertexAttribPointer(VertexBufferIndex,
						GetShaderDataTypeElementCount(ShaderDataType),
						ShaderDataTypeToOpenGLBaseType(ShaderDataType),
						ElementIt->bNormalized ? GL_TRUE : GL_FALSE,
						VertexBufferLayout.GetStride(),
						(const void*)ElementIt->Offset);

					++VertexBufferIndex;

					break;
				}
				case EShaderDataType::Mat3: FALLTHROUGH;
				case EShaderDataType::Mat4:
				{
					uint8_t ElementCount = GetShaderDataTypeElementCount(ShaderDataType);

					for (uint8_t i = 0; i < ElementCount; ++i)
					{
						glEnableVertexAttribArray(VertexBufferIndex);

						glVertexAttribPointer(VertexBufferIndex,
							ElementCount,
							ShaderDataTypeToOpenGLBaseType(ShaderDataType),
							ElementIt->bNormalized ? GL_TRUE : GL_FALSE,
							VertexBufferLayout.GetStride(),
							(const void*)(ElementIt->Offset + sizeof(float) * ElementCount * i));

						glVertexAttribDivisor(VertexBufferIndex, 1);

						++VertexBufferIndex;
					}

					break;
				}
				default:
				{
					BS_ASSERT(false, "Attempting to add VertexBufferElement with unknown ShaderDataType to VertexArray");
					break;
				}
			}
		}

		VertexBuffer->Unbind();

		glBindVertexArray(0);

		VertexBuffers.push_back(VertexBuffer);
		VertexCount += VertexBuffer->GetVertexCount();
	}

	void FOpenGLVertexArray::SetIndexBuffer(IIndexBuffer* const IndexBuffer)
	{
		BS_PROFILE_FUNCTION();

		glBindVertexArray(UniqueID);

		IndexBuffer->Bind();

		this->IndexBuffer = IndexBuffer;

		IndexBuffer->Unbind();

		glBindVertexArray(0);
	}
}
