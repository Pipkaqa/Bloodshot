#pragma once

#include "Core.h"

#include "Shader.h"

namespace Bloodshot
{
	class IVertexArray;
	struct FSubMeshInfo;

	namespace Private
	{
		struct FRenderingSystem;
	}

	enum class ERendererType : uint8_t
	{
		OpenGL = 0,
		Vulkan,
	};

	class IRenderer : public TManager<IRenderer>
	{
		friend class IEngineContext;
		friend class FEngineEditorContext;
		friend class FEngineGameContext;
		friend struct Private::FRenderingSystem;
		friend class FMeshComponent;

	public:
		virtual ~IRenderer() {}

		NODISCARD static ERendererType GetType() noexcept;

	protected:
		IRenderer(ERendererType Type, const glm::vec4& BackgroundColor);

		ERendererType Type;
		glm::vec4 BackgroundColor;

		TUniquePtr<IShader> DefaultShader = nullptr;

		virtual void DrawTriangles(const TReference<IVertexArray> VertexArray) = 0;
		virtual void DrawIndexed(const TReference<IVertexArray> VertexArray) = 0;
		virtual void DrawPart(const TReference<IVertexArray> VertexArray, const FSubMeshInfo& Part) = 0;
		virtual void DrawLines() = 0;
		virtual void ClearBackground() = 0;

		NODISCARD FORCEINLINE TReference<IShader> GetDefaultShader() noexcept
		{
			return Instance->DefaultShader.GetReference();
		}
	};
}
