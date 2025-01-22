#include "SceneManager.h"

namespace Bloodshot
{
	FSceneManager::FSceneManager()
	{
		Instance = this;
	}

	void FSceneManager::Init()
	{
		BS_LOG(Debug, "Creating SceneManager...");
	}

	void FSceneManager::Dispose()
	{
		BS_LOG(Debug, "Destroying SceneManager...");
	}

	void FSceneManager::LoadScene(const InstanceID_t Index)
	{
		Instance->EndPlay();

		FSceneMap& Scenes = Instance->SceneMap;

		const FSceneMap::iterator& It = Scenes.find(Index);

		BS_LOG_IF(It != Scenes.end(), Error, "Attempting to load not existing Scene, index: {}", Index);

		Instance->CurrentScene = &It->second;

		Instance->BeginPlay();
	}

	void FSceneManager::AddScene()
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "Attempting to create Scene in runtime");

		static InstanceID_t UniqueID = 0;

		BS_LOG(Debug, "Creating Scene with index: {}...", UniqueID);

		SceneMap.emplace(UniqueID, UniqueID);

		++UniqueID;
	}

	void FSceneManager::SetStartingScene(const InstanceID_t Idx)
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "Attempting to set starting Scene in runtime");

		const FSceneMap::iterator& It = SceneMap.find(Idx);

		BS_ASSERT(It != SceneMap.end(), "Attempting to set not existing Scene as starting");

		CurrentScene = &It->second;
	}

	void FSceneManager::BeginPlay()
	{
		BS_ASSERT(CurrentScene, "Active Scene not found");

		BS_LOG(Debug, "Begin playing on Scene with index: {}...", CurrentScene->InstanceID);

		CurrentScene->BeginPlay();
	}

	void FSceneManager::EndPlay()
	{
		BS_ASSERT(CurrentScene, "Active Scene not found");

		BS_LOG(Debug, "End playing on Scene with index: {}...", CurrentScene->InstanceID);

		CurrentScene->EndPlay();
	}

	void FSceneManager::Tick(float DeltaTime)
	{
		BS_ASSERT(CurrentScene, "Active Scene not found");

		CurrentScene->Tick(DeltaTime);
	}
}
