#pragma once

#include "Component.h"
#include "MathLibrary.h"
#include "PerspectiveCamera.h"
#include "Platform.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	class FCameraComponent final : public IComponent, public IPerspectiveCamera
	{
	public:
		FCameraComponent(const float Fov, const float AspectRatio, const float Near, const float Far);

		NODISCARD FORCEINLINE glm::mat4 GetViewMatrix() const
		{
			// BSTODO: Implement:
					
					//glm::vec3& Position = TransformComponent->Position;
					//glm::vec3& Rotation = TransformComponent->Rotation;
					//float Theta = glm::radians(eulers.z);
					//float phi = glm::radians(eulers.y);
					//
					//glm::vec3& right = cameraComponent.right;
					//glm::vec3& up = cameraComponent.up;
					//glm::vec3& forwards = cameraComponent.forwards;
					//
					//forwards = {
					//	glm::cos(theta) * glm::cos(phi),
					//	glm::sin(theta) * glm::cos(phi),
					//	glm::sin(phi)
					//};
					//right = glm::normalize(glm::cross(forwards, global_up));
					//up = glm::normalize(glm::cross(right, forwards));
					//
					//glm::mat4 view = glm::lookAt(pos, pos + forwards, up);

			return glm::mat4(1.f);
		}

	private:
		virtual void BeginPlay() override;
		virtual void EndPlay() override;

		FTransformComponent* TransformComponent = nullptr;
	};
}
