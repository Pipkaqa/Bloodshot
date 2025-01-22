#pragma once

#include "Core.h"

#include "Scene.h"

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FSceneManager final : public TSingleton<FSceneManager>
	{
	public:
		using FSceneMap = TUnorderedMap<InstanceID_t, FScene>;

		FSceneManager();

		FSceneMap SceneMap;
		TReference<FScene> CurrentScene = nullptr;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD FORCEINLINE static TReference<FScene> GetCurrentScene()
		{
			return Instance->CurrentScene;
		}

		static void LoadScene(const InstanceID_t Idx);

		void AddScene();
		void SetStartingScene(const InstanceID_t Idx);

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
