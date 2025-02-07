#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IRenderer;

	namespace Private
	{
		struct FRenderingSystem final
		{
			void Execute(float DeltaTime, TReference<IRenderer> Renderer);
		};
	}
}
