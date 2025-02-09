#pragma once

#include "Core.h"

#include "Handle.h"
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
		using FSceneMap = TUnorderedMap<FInstanceID, FScene>;

		NODISCARD static TReference<FScene> GetCurrentScene();

		static void LoadScene(const FInstanceID Index);

	private:
		FSceneManager();

		static inline FSceneManager* Instance = nullptr;

		FSceneMap SceneMap;
		TReference<FScene> CurrentScene = nullptr;

		virtual void Init() override;
		virtual void Dispose() override;

		void AddScene();
		void SetStartingScene(const FInstanceID Index);

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
