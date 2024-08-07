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

		for (FSceneManager::FTypeIDSceneUnorderedMap::value_type ScenePair : ScenesUnorderedMap)
		{
			IScene* const Scene = ScenePair.second;

			if (!Scene) continue;

			BS_LOG(Trace, "Destroying Scene of type: {0}...", Scene->GetTypeName());

			delete Scene;
		}

		ScenesUnorderedMap.clear();
	}

	void FSceneManager::BeginSimulation()
	{
		BS_ASSERT(CurrentScenePtr, "Active Scene not found");

		CurrentScenePtr->BeginSimulation();
	}

	void FSceneManager::EndSimulation()
	{
		BS_ASSERT(CurrentScenePtr, "Active Scene not found");

		CurrentScenePtr->EndSimulation();
	}

	void FSceneManager::InternalBeginPlay()
	{
		BS_ASSERT(CurrentScenePtr, "Active Scene not found");
		BS_LOG(Debug, "Begin playing on Scene of type: {0}...", CurrentScenePtr->GetTypeName());

		CurrentScenePtr->InternalBeginPlay();
		CurrentScenePtr->BeginPlay();
		//CurrentScenePtr->NoticeBeginPlay(); Notice all entities, components and systems about BeginPlay (in the future, when editor will be created)
	}

	void FSceneManager::InternalEndPlay()
	{
		BS_ASSERT(CurrentScenePtr, "Active Scene not found");
		BS_LOG(Debug, "End playing on Scene of type: {0}...", CurrentScenePtr->GetTypeName());

		//CurrentScenePtr->NoticeEndPlay(); Notice all entities, components and systems about EndPlay (in the future, when editor will be created)
		CurrentScenePtr->EndPlay();
		CurrentScenePtr->InternalEndPlay();
	}

	void FSceneManager::InternalUpdate(float DeltaTime, TUniquePtr<IRenderer>& Renderer, TUniquePtr<IWindow>& Window)
	{
		BS_ASSERT(CurrentScenePtr, "Active Scene not found");

		CurrentScenePtr->InternalUpdate(DeltaTime, Renderer, Window);
	}
}
