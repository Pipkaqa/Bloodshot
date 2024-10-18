#pragma once

#include "MathLibrary.h"
#include "Platform.h"
#include "Templates/Singleton.h"

#include <cstdint>

namespace Bloodshot
{
	class IShader;
	class IVertexArray;

	enum class ERendererType : uint8_t
	{
		OpenGL = 0,
		Vulkan,
	};

	class IRenderer abstract : public TSingleton<IRenderer>
	{
		friend class FMeshComponent;

	public:
		IRenderer(ERendererType Type, const glm::vec4& BackgroundColor);
		virtual ~IRenderer() {}

		ERendererType Type;
		glm::vec4 BackgroundColor;

		IShader* DefaultShader = nullptr;

		virtual void DrawTriangles(const IVertexArray* const VertexArray) = 0;
		virtual void DrawIndexed(const IVertexArray* const VertexArray) = 0;
		virtual void DrawLines() = 0;
		virtual void ClearBackground() = 0;

		NODISCARD FORCEINLINE static ERendererType GetType() noexcept
		{
			return Instance->Type;
		}

		NODISCARD FORCEINLINE IShader* GetDefaultShader() noexcept
		{
			return Instance->DefaultShader;
		}
	};
}
