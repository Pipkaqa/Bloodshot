#include "CameraComponent.h"
#include "ECS.h"

namespace Bloodshot
{
	FCameraComponent::FCameraComponent(const float Fov, const float AspectRatio, const float Near, const float Far)
		: IPerspectiveCamera(Fov, AspectRatio, Near, Far)
	{
	}

	void FCameraComponent::BeginPlay()
	{
		TransformComponent = IECS::GetComponent<FTransformComponent>(Owner);
	}

	void FCameraComponent::EndPlay()
	{
	}
}
