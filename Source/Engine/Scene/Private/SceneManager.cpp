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

	void FSceneManager::LoadScene(const InstanceID_t Idx)
	{
		Instance->EndPlay();

		FTypeIDSceneUnorderedMap& Scenes = Instance->ScenesUnorderedMap;

		const FTypeIDSceneUnorderedMap::iterator& It = Scenes.find(Idx);

		BS_LOG_IF(It != Scenes.end(), Error, "Attempting to load not existing Scene, index: {0}", Idx);

		Instance->CurrentScene = &It->second;

		Instance->BeginPlay();
	}

	void FSceneManager::AddScene()
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "Attempting to create Scene in runtime");

		static InstanceID_t Idx = 0;

		BS_LOG(Debug, "Creating Scene with index: {0}...", Idx);

		Instance->ScenesUnorderedMap.emplace(Idx, Idx);

		++Idx;
	}

	void FSceneManager::SetStartingScene(const InstanceID_t Idx)
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "Attempting to set starting Scene in runtime");

		FTypeIDSceneUnorderedMap& Scenes = Instance->ScenesUnorderedMap;

		const FTypeIDSceneUnorderedMap::iterator& It = Scenes.find(Idx);

		BS_ASSERT(It != Scenes.end(), "Attempting to set not existing Scene as starting");

		Instance->CurrentScene = &It->second;
	}

	void FSceneManager::BeginPlay()
	{
		FScene* const CurrentScene = Instance->CurrentScene;

		BS_ASSERT(CurrentScene, "Active Scene not found");

		BS_LOG(Debug, "Begin playing on Scene with index: {0}...", CurrentScene->InstanceID);

		CurrentScene->BeginPlay();
	}

	void FSceneManager::EndPlay()
	{
		FScene* const CurrentScene = Instance->CurrentScene;

		BS_ASSERT(CurrentScene, "Active Scene not found");

		BS_LOG(Debug, "End playing on Scene with index: {0}...", CurrentScene->InstanceID);

		CurrentScene->EndPlay();
	}

	void FSceneManager::Tick(float DeltaTime)
	{
		BS_ASSERT(CurrentScene, "Active Scene not found");

		CurrentScene->Tick(DeltaTime);
	}
}
