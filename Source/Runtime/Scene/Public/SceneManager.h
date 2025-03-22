#pragma once

#include "Core.h"

#include "Scene.h"

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FSceneManager final
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;

	public:
		NODISCARD static FSceneManager& GetInstance();

		NODISCARD FORCEINLINE static FScene* GetCurrentScene()
		{
			return GetInstance().CurrentScene;
		}

		static void LoadScene(const size_t Index);

	private:
		FSceneManager() {}

		TArray<FScene*> Scenes;
		FScene* CurrentScene;

		void AddScene();
		void SetStartingScene(const size_t Index);

		void BeginPlay();
		void EndPlay();
		void Tick(float DeltaTime);
	};
}
