#pragma once

#include "Core.h"

#include "Scene.h"

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FSceneManager final
	{
		friend class Private::IEngineContext;
		friend class Private::FEngineEditorContext;
		friend class Private::FEngineGameContext;

	public:
		NODISCARD static FSceneManager& GetInstance();

		NODISCARD FORCEINLINE static TReference<FScene> GetCurrentScene()
		{
			return GetInstance().CurrentScene;
		}

		static void LoadScene(const size_t Index);

	private:
		FSceneManager() {}

		TArray<TReference<FScene>> Scenes;
		TReference<FScene> CurrentScene;

		void AddScene();
		void SetStartingScene(const size_t Index);

		void BeginPlay();
		void EndPlay();
		void Tick(float DeltaTime);
	};
}
