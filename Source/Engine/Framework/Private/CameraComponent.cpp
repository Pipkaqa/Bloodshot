#include "CameraComponent.h"
#include "ECS.h"

namespace Bloodshot
{
	FCameraComponent::FCameraComponent(const FCameraSettings& Settings)
		: ICamera(Settings)
	{
	}

	void FCameraComponent::BeginPlay()
	{
		// BSTODO: Rewrite code

		TransformComponent = IECS::GetComponent<FTransformComponent>(Owner);

		LastPosition = TransformComponent->Position;
		LastRotation = TransformComponent->Rotation;

		(void)GetViewMatrix();
	}

	void FCameraComponent::EndPlay()
	{
	}
}
