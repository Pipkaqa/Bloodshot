#include "Components/CameraComponent.h"
#include "ComponentManager.h"

namespace Bloodshot
{
	FCameraComponent::FCameraComponent(const FCameraSettings& Settings)
		: ICamera(Settings)
	{
	}

	void FCameraComponent::BeginPlay()
	{
		// BSTODO: Rewrite code

		TransformComponent = FComponentManager::GetComponent<FTransformComponent>(Owner);

		LastPosition = TransformComponent->Position;
		LastRotation = TransformComponent->Rotation;

		(void)GetViewMatrix();
	}

	void FCameraComponent::EndPlay()
	{
	}
}
