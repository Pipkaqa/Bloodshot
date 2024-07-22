#pragma once

#include "ECS/Component.h"
#include "Math/Math.h"

namespace Bloodshot
{
	class TransformComponent final : public Component<TransformComponent>
	{
	};

	class SpriteComponent final : public Component<TransformComponent>
	{
	};
}
