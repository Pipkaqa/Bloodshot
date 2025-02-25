#include "TransformComponent.h"

namespace Bloodshot
{
	void FTransformComponent::UpdateVectors() const
	{
		const glm::vec3& Rotation = Transform.Rotation;

		const float Theta = glm::radians(Rotation.x);
		const float Phi = glm::radians(Rotation.y);
		const float Radius = glm::radians(Rotation.z);

		const float SinTheta = glm::sin(Theta);
		const float CosTheta = glm::cos(Theta);
		const float SinPhi = glm::sin(Phi);
		const float CosPhi = glm::cos(Phi);

		ForwardVector = glm::normalize(glm::vec3(CosTheta * CosPhi, SinPhi, SinTheta * CosPhi));
		RightVector = glm::normalize(glm::cross(ForwardVector, IVector3Constants::Up));
		UpVector = glm::normalize(glm::cross(RightVector, ForwardVector));
		bDirty = false;
	}
}