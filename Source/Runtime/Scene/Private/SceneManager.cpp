#include "SceneManager.h"

namespace Bloodshot
{
	FSceneManager& FSceneManager::GetInstance()
	{
		static FSceneManager Instance;
		return Instance;
	}

	void FSceneManager::LoadScene(const size_t Index)
	{
		FSceneManager& Instance = GetInstance();
		TArray<TReference<FScene>>& Scenes = Instance.Scenes;

		if (Index >= Scenes.GetSize())
		{
			BS_LOG(Error, "Trying to load not existing scene with index: {}", Index);
			return;
		}

		TReference<FScene>& CurrentScene = Instance.CurrentScene;
		if (CurrentScene) Instance.EndPlay();
		CurrentScene = Scenes[Index];
		Instance.BeginPlay();
	}

	void FSceneManager::AddScene()
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "FSceneManager::AddScene: Trying to create scene in runtime");
		TReference<FScene> Scene = NewObject<FScene>();
		Scenes.EmplaceBack(Scene);
	}

	void FSceneManager::SetStartingScene(const size_t Index)
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "FSceneManager::SetStartingScene: Trying to set starting scene in runtime");
		BS_ASSERT(Index >= Scenes.GetSize(), "FSceneManager::SetStartingScene: Trying to change starting scene to not existing scene");
		CurrentScene = Scenes[Index];
	}

	void FSceneManager::BeginPlay()
	{
		BS_ASSERT(CurrentScene, "FSceneManager::BeginPlay: Current scene is not set");
		BS_LOG(Debug, "Beginning play on scene with index: {}...", Scenes.Find(CurrentScene));
		CurrentScene->BeginPlay();
	}

	void FSceneManager::EndPlay()
	{
		BS_ASSERT(CurrentScene, "FSceneManager::EndPlay: Current scene is not set");
		BS_LOG(Debug, "Ending play on FScene with index: {}...", Scenes.Find(CurrentScene));
		CurrentScene->EndPlay();
	}

	void FSceneManager::Tick(float DeltaTime)
	{
		BS_ASSERT(CurrentScene, "FSceneManager::Tick: Current scene is not set");
		CurrentScene->Tick(DeltaTime);
	}
}
