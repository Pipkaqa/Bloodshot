#pragma once

#include "Core.h"

#include "Scene.h"

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FSceneManager final : public TManager<FSceneManager>
	{
		friend class IEngineContext;
		friend class FEngineEditorContext;
		friend class FEngineGameContext;

	public:
		using FSceneMap = TUnorderedMap<InstanceID_t, FScene>;

		NODISCARD static TReference<FScene> GetCurrentScene();

		static void LoadScene(const InstanceID_t Index);

	private:
		FSceneManager();

		static inline FSceneManager* Instance = nullptr;

		FSceneMap SceneMap;
		TReference<FScene> CurrentScene = nullptr;

		virtual void Init() override;
		virtual void Dispose() override;

		void AddScene();
		void SetStartingScene(const InstanceID_t Index);

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
