#include "CameraComponent.h"
#include "Entity.h"

namespace Bloodshot
{
	void FCameraComponent::BeginPlay()
	{
		TransformComponent = Owner->GetComponent<FTransformComponent>();
		TransformComponent->OnTransformChanged.Subscribe(GetUniqueID(), [this]()
		{
			UpdateViewMatrix();
		});
		UpdateViewMatrix();
	}

	void FCameraComponent::EndPlay()
	{
		TransformComponent->OnTransformChanged.Unsubscribe(GetUniqueID());
	}

	void FCameraComponent::UpdateViewMatrix()
	{
		const glm::vec3& Position = TransformComponent->GetPosition();
		const glm::vec3& Rotation = TransformComponent->GetRotation();

		const float Theta = glm::radians(Rotation.x);
		const float Phi = glm::radians(Rotation.y);
		const float Radius = glm::radians(Rotation.z);

		const float SinTheta = glm::sin(Theta);
		const float CosTheta = glm::cos(Theta);
		const float SinPhi = glm::sin(Phi);
		const float CosPhi = glm::cos(Phi);

		//BS_LOG(Warning, "Phi: {0}; SinPhi: {1}; CosPhi: {2}", Phi, SinPhi, CosPhi);
		// BSTODO: Rewrite correctly, radius does nothing if > 0
		//const glm::vec3& CameraForwardVec = Radius * glm::vec3(CosTheta * CosPhi, SinPhi, SinTheta * CosPhi);
		const glm::vec3& CameraForwardVec = glm::normalize(glm::vec3(CosTheta * CosPhi, SinPhi, SinTheta * CosPhi));
		const glm::vec3& CameraRightVec = glm::normalize(glm::cross(CameraForwardVec, IVector3Constants::Up));
		const glm::vec3& CameraUpVec = glm::normalize(glm::cross(CameraRightVec, CameraForwardVec));

		ViewMatrix = glm::lookAt(Position, Position + CameraForwardVec, CameraUpVec);
	}
}
