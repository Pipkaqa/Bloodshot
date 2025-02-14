#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IRenderer;

	namespace Private
	{
		class FRenderingSystem final
		{
			friend class IEngineContext;
			friend class FEngineEditorContext;
			friend class FEngineGameContext;

			static void Execute(float DeltaTime, TReference<IRenderer> Renderer);
		};
	}
}
