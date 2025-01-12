#pragma once

#include "CameraComponent.h"
#include "Platform/Platform.h"
#include "RenderingSystem.h"
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"
#ifdef BS_NETWORKING_ON
#include "NetworkingSystem.h"
#endif

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FScene final
	{
		friend class FSceneManager;

	public:
		FScene(const InstanceID_t InstanceID);

		TReference<FCameraComponent> MainCameraComponent = nullptr;

		std::vector<TReference<FCameraComponent>> CameraVec;

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
