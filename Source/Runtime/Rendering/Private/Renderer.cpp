#include "Renderer.h"

namespace Bloodshot
{
	IRenderer* IRenderer::GetInstance()
	{
		return Instance;
	}

	ERendererType IRenderer::GetType() noexcept
	{
		return Instance->Type;
	}
}
