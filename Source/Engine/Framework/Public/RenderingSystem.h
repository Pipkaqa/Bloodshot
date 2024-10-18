#pragma once

#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	class IRenderer;

	namespace Private
	{
		struct FRenderingSystem final
		{
			void Execute(float DeltaTime, TUniquePtr<IRenderer>& Renderer);
		};
	}
}
