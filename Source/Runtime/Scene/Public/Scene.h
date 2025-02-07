#pragma once

#include "Core.h"

#include "CameraComponent.h"

namespace Bloodshot
{
	class FScene final
	{
		friend class FSceneManager;

	public:
		FScene(const InstanceID_t InstanceID);

		TReference<FCameraComponent> MainCameraComponent = nullptr;

		NODISCARD FORCEINLINE InstanceID_t GetInstanceID() const noexcept
		{
			return InstanceID;
		}

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);

	private:
		InstanceID_t InstanceID;
	};
}
