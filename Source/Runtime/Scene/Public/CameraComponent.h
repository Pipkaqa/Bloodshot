#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable 'ViewMatrix' is uninitialized

#include "Core.h"

#include "Camera.h"
#include "Component.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	BSCLASS();
	class FCameraComponent final : public IComponent, public ICamera
	{
		GENERATED_BODY();

	public:
		FORCEINLINE FCameraComponent(const FCameraSettings& Settings = FCameraSettings())
			: ICamera(Settings)
		{
		}

		NODISCARD FORCEINLINE const glm::mat4& GetViewMatrix()
		{
			return ViewMatrix;
		}

		void UpdateViewMatrix();

	private:
		virtual void BeginPlay() override;
		virtual void EndPlay() override;

		TReference<FTransformComponent> TransformComponent = nullptr;

		glm::mat4 ViewMatrix;
	};
}

#pragma warning(pop)
