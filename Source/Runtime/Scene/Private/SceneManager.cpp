#include "SceneManager.h"

namespace Bloodshot
{
	FSceneManager::FSceneManager()
	{
		Instance = this;
	}

	void FSceneManager::Init()
	{
		BS_LOG(Debug, "Creating FSceneManager...");
	}

	void FSceneManager::Dispose()
	{
		BS_LOG(Debug, "Destroying FSceneManager...");
	}

	TReference<FScene> FSceneManager::GetCurrentScene()
	{
		return Instance->CurrentScene;
	}

	void FSceneManager::LoadScene(const InstanceID_t Index)
	{
		Instance->EndPlay();

		FSceneMap& Scenes = Instance->SceneMap;

		const FSceneMap::iterator& It = Scenes.find(Index);

		BS_LOG_IF(It != Scenes.end(), Error, "Trying to load not existing scene with index: {}", Index);

		Instance->CurrentScene = &It->second;

		Instance->BeginPlay();
	}

	void FSceneManager::AddScene()
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "FSceneManager::AddScene: Trying to create scene in runtime");

		static InstanceID_t UniqueID = 0;

		BS_LOG(Debug, "Creating FScene with index: {}...", UniqueID);

		SceneMap.emplace(UniqueID, UniqueID);

		++UniqueID;
	}

	void FSceneManager::SetStartingScene(const InstanceID_t Index)
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "FSceneManager::SetStartingScene: Trying to set CurrentScene in runtime");

		const FSceneMap::iterator& It = SceneMap.find(Index);

		BS_ASSERT(It != SceneMap.end(), "FSceneManager::SetStartingScene: Trying to change CurrentScene to not existing scene");

		CurrentScene = &It->second;
	}

	void FSceneManager::BeginPlay()
	{
		BS_ASSERT(CurrentScene, "FSceneManager::BeginPlay: CurrentScene not set");

		BS_LOG(Debug, "Beginning play on Scene with index: {}...", CurrentScene->InstanceID);

		CurrentScene->BeginPlay();
	}

	void FSceneManager::EndPlay()
	{
		BS_ASSERT(CurrentScene, "FSceneManager::EndPlay: CurrentScene not set");

		BS_LOG(Debug, "Ending play on FScene with index: {}...", CurrentScene->InstanceID);

		CurrentScene->EndPlay();
	}

	void FSceneManager::Tick(float DeltaTime)
	{
		BS_ASSERT(CurrentScene, "FSceneManager::Tick: CurrentScene not set");

		CurrentScene->Tick(DeltaTime);
	}
}
