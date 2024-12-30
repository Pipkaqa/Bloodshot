#pragma once

#include "Camera.h"
#include "Component.h"
#include "MathLibrary.h"
#include "Platform.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	class FCameraComponent final : public IComponent, public ICamera
	{
	public:
		FCameraComponent(const FCameraSettings& Settings = {});

		// BSTODO: Caching

		NODISCARD FORCEINLINE const glm::mat4& GetViewMatrix()
		{
			const glm::vec3& Position = TransformComponent->Position;
			const glm::vec3& Rotation = TransformComponent->Rotation;

			if (LastPosition != Position || LastRotation != Rotation)
			{
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

				LastPosition = Position;
				LastRotation = Rotation;

				ViewMatrix = glm::lookAt(Position, Position + CameraForwardVec, CameraUpVec);
			}
			
			return ViewMatrix;
		}

	private:
		virtual void BeginPlay() override;
		virtual void EndPlay() override;

		TReference<const FTransformComponent> TransformComponent = nullptr;

		glm::vec3 LastPosition;
		glm::vec3 LastRotation;

		glm::mat4 ViewMatrix;
	};
}
