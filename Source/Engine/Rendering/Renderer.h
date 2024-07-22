#pragma once

#include "Core/EngineFramework.h"
#include "GameFramework/World.h"
#include "Math/Math.h"
#include "Utility/ISingleton.h"

namespace Bloodshot
{
	class Renderer abstract : public ISingleton<Renderer>
	{
		OWNED_BY_CORE;
		RENDERING_PART;

	public:
		enum class Type : uint8_t
		{
			OpenGL = 0,
		};

		struct Config final
		{
			Type m_Type = Type::OpenGL;

			const glm::ivec4 m_BackgroundColor = {0, 1, 0, 1};
		};

		virtual void DrawTriangles() = 0;
		virtual void DrawLines() = 0;
		virtual void ClearBackground() = 0;

		NODISCARD FORCEINLINE static Type GetType()
		{
			return s_Instance->m_Config->m_Type;
		}

	private:
		using ISingleton::Create;

		const Config* m_Config;

		NODISCARD static Renderer* Create(const Config& config);
	};
}
