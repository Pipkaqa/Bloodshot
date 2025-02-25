#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IVertexArray;
	struct FSubMeshInfo;
	namespace Private::Rendering { class FRenderingSystem; }

	enum class ERendererType : uint8_t
	{
		OpenGL = 0,
		Vulkan,
	};

	class IRenderer
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;
		friend class Private::Rendering::FRenderingSystem;
		friend class FMeshComponent;

	public:
		virtual ~IRenderer() {}

		NODISCARD static IRenderer* GetInstance();
		NODISCARD static ERendererType GetType() noexcept;

	protected:
		IRenderer(ERendererType Type, const glm::vec4& BackgroundColor)
			: Type(Type)
			, BackgroundColor(BackgroundColor)
		{
		}

		static inline IRenderer* Instance = nullptr;

		ERendererType Type;
		glm::vec4 BackgroundColor;

		virtual void Init() = 0;
		virtual void Dispose() = 0;

		virtual void DrawTriangles(const TReference<IVertexArray> VertexArray) = 0;
		virtual void DrawIndexed(const TReference<IVertexArray> VertexArray) = 0;
		virtual void DrawPart(const TReference<IVertexArray> VertexArray, const FSubMeshInfo& Part) = 0;
		virtual void DrawLines() = 0;
		virtual void ClearBackground() = 0;
	};
}
