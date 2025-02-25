#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IRenderer;

	namespace Private::Rendering
	{
		class FRenderingSystem final
		{
			friend class Private::Launch::IEngineContext;
			friend class Private::Launch::FEngineEditorContext;
			friend class Private::Launch::FEngineGameContext;

			static void Execute(float DeltaTime, TReference<IRenderer> Renderer);
		};
	}
}
