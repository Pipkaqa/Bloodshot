#pragma once

#include "Core.h"

#include "CameraComponent.h"
#include "Handle.h"

namespace Bloodshot
{
	namespace Private
	{
		struct FRenderingSystem;
	}

	class FScene final
	{
		friend class FSceneManager;
		friend struct Private::FRenderingSystem;

	public:
		FScene(const FInstanceID InstanceID);

	private:
		FInstanceID InstanceID;
		TReference<FCameraComponent> MainCameraComponent = nullptr;

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
