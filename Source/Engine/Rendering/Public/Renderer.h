#pragma once

#include "MathLibrary.h"
#include "Platform/Platform.h"
#include "Shader.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

#include <cstdint>

namespace Bloodshot
{
	class IVertexArray;
	struct FSubMeshInfo;

	enum class ERendererType : uint8_t
	{
		OpenGL = 0,
		Vulkan,
	};

	class IRenderer : public TSingleton<IRenderer>
	{
		friend class FMeshComponent;

	public:
		IRenderer(ERendererType Type, const glm::vec4& BackgroundColor);
		virtual ~IRenderer() {}

		ERendererType Type;
		glm::vec4 BackgroundColor;

		TUniquePtr<IShader> DefaultShader = nullptr;

		virtual void DrawTriangles(const TReference<IVertexArray> VertexArray) = 0;
		virtual void DrawIndexed(const TReference<IVertexArray> VertexArray) = 0;
		virtual void DrawPart(const TReference<IVertexArray> VertexArray, const FSubMeshInfo& Part) = 0;
		virtual void DrawLines() = 0;
		virtual void ClearBackground() = 0;

		NODISCARD FORCEINLINE static ERendererType GetType() noexcept
		{
			return Instance->Type;
		}

		NODISCARD FORCEINLINE TReference<IShader> GetDefaultShader() noexcept
		{
			return Instance->DefaultShader.GetReference();
		}
	};
}
