#include "Misc/AssertionMacros.h"
#include "OpenGL/OpenGLHeader.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "Platform/Platform.h"
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

	void FOpenGLVertexArray::AddVertexBuffer(TUniquePtr<IVertexBuffer>&& VertexBuffer)
	{
		BS_PROFILE_FUNCTION();

		const FVertexBufferLayout& VertexBufferLayout = VertexBuffer->GetLayout();

		BS_ASSERT(VertexBufferLayout.GetElements().GetSize(), "Attempting to add VertexBuffer without Layout");

		glBindVertexArray(UniqueID);
		VertexBuffer->Bind();

		using FElementConstIterator = FVertexBufferLayout::FElementArray::FConstIterator;

		for (FElementConstIterator ElementConstIt = VertexBufferLayout.CreateConstIterator(); ElementConstIt; ++ElementConstIt)
		{
			const EShaderDataType ShaderDataType = ElementConstIt->Type;
			const uint8_t ShaderElementCount = GetShaderDataTypeElementCount(ShaderDataType);
			const GLenum ShaderOpenGLType = ShaderDataTypeToOpenGLBaseType(ShaderDataType);
			const GLboolean bElementNormalized = ElementConstIt->bNormalized ? GL_TRUE : GL_FALSE;
			const uint16_t LayoutStride = VertexBufferLayout.GetStride();
			const void* const ElementOffset = (const void*)ElementConstIt->Offset;

			switch (ShaderDataType)
			{
				case EShaderDataType::Bool: FALLTHROUGH;
				case EShaderDataType::Int: FALLTHROUGH;
				case EShaderDataType::Int2: FALLTHROUGH;
				case EShaderDataType::Int3: FALLTHROUGH;
				case EShaderDataType::Int4:
				{
					glEnableVertexAttribArray(VertexBufferIndex);
					glVertexAttribIPointer(VertexBufferIndex++,
						ShaderElementCount,
						ShaderOpenGLType,
						LayoutStride,
						ElementOffset);

					break;
				}
				case EShaderDataType::Float: FALLTHROUGH;
				case EShaderDataType::Float2: FALLTHROUGH;
				case EShaderDataType::Float3: FALLTHROUGH;
				case EShaderDataType::Float4:
				{
					glEnableVertexAttribArray(VertexBufferIndex);
					glVertexAttribPointer(VertexBufferIndex++,
						ShaderElementCount,
						ShaderOpenGLType,
						bElementNormalized,
						LayoutStride,
						ElementOffset);

					break;
				}
				case EShaderDataType::Mat3: FALLTHROUGH;
				case EShaderDataType::Mat4:
				{
					for (uint8_t i = 0; i < ShaderElementCount; ++i)
					{
						glEnableVertexAttribArray(VertexBufferIndex);
						glVertexAttribPointer(VertexBufferIndex,
							ShaderElementCount,
							ShaderOpenGLType,
							bElementNormalized,
							LayoutStride,
							(const void*)(*(uint16_t*)ElementOffset + sizeof(float) * ShaderElementCount * i));

						glVertexAttribDivisor(VertexBufferIndex++, 1);
					}

					break;
				}
				default:
				{
					BS_ASSERT(false, "Trying to add FVertexBufferElement with unknown EShaderDataType to OpenGLVertexArray");
					break;
				}
			}
		}

		VertexBuffer->Unbind();
		glBindVertexArray(0);

		VertexCount += VertexBuffers.EmplaceBackGetRef(std::move(VertexBuffer))->GetVertexCount();
	}

	void FOpenGLVertexArray::SetIndexBuffer(TUniquePtr<IIndexBuffer>&& NewIndexBuffer)
	{
		BS_PROFILE_FUNCTION();

		glBindVertexArray(UniqueID);
		NewIndexBuffer->Bind();

		IndexBuffer = std::move(NewIndexBuffer);

		IndexBuffer->Unbind();
		glBindVertexArray(0);
	}
}
