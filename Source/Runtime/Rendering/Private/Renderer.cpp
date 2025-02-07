#include "Renderer.h"

namespace Bloodshot
{
	IRenderer::IRenderer(ERendererType Type, const glm::vec4& BackgroundColor)
		: Type(Type)
		, BackgroundColor(BackgroundColor)
	{
		Instance = this;
	}

	ERendererType IRenderer::GetType() noexcept
	{
		return Instance->Type;
	}
}
