#pragma once

#include "Core.h"

#include "CameraComponent.h"

namespace Bloodshot
{
	namespace Private
	{
		class FRenderingSystem;
	}

	BSCLASS();
	class FScene final : public IObject
	{
		friend class FSceneManager;
		friend class Private::FRenderingSystem;

		GENERATED_BODY();

		TReference<FCameraComponent> MainCameraComponent = nullptr;

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
