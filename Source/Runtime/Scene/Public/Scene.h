#pragma once

#include "Core.h"

#include "CameraComponent.h"

namespace Bloodshot
{
	namespace Private::Rendering { class FRenderingSystem; }

	BSCLASS();
	class FScene final : public IObject
	{
		friend class FSceneManager;
		friend class Private::Rendering::FRenderingSystem;

		GENERATED_BODY();

	public:
		static void SetMainCamera(FCameraComponent* CameraComponent);
		static void SetMainCamera(FEntity* EntityWithCameraComponent);

	private:
		FCameraComponent* MainCameraComponent = nullptr;

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
