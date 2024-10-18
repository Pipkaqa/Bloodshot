#pragma once

#include "Component.h"
#include "MathLibrary.h"

namespace Bloodshot
{
	class FTransformComponent final : public IComponent, public FTransform
	{
	};
}
