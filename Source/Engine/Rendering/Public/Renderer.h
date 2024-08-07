#pragma once

#include "MathLibrary.h"
#include "Platform.h"
#include "Templates/Singleton.h"

#include <cstdint>

namespace Bloodshot
{
	enum class ERendererType : uint8_t
	{
		OpenGL = 0U,
		Vulkan,
	};

	class IRenderer abstract : public TSingleton<IRenderer>
	{
	public:
		IRenderer(ERendererType Type, const glm::uvec4& BackgroundColor = {0U, 1U, 0U, 1U})
			: Type(Type), BackgroundColor(BackgroundColor)
		{
			Instance = this;
		}

		ERendererType Type;
		glm::uvec4 BackgroundColor;

		virtual void DrawTriangles() = 0;
		virtual void DrawLines() = 0;
		virtual void ClearBackground() = 0;

		NODISCARD FORCEINLINE static ERendererType GetType()
		{
			return Instance->Type;
		}
	};
}
