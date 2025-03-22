#include "Scene.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "System.h"
#include "SystemManager.h"

namespace Bloodshot
{
	void FScene::SetMainCamera(FCameraComponent* CameraComponent)
	{
		FScene* Me = FSceneManager::GetCurrentScene();
		Me->MainCameraComponent = CameraComponent;
	}

	void FScene::SetMainCamera(FEntity* Camera)
	{
		FScene* Me = FSceneManager::GetCurrentScene();
		Me->MainCameraComponent = Camera->GetComponent<FCameraComponent>();
	}

	void FScene::BeginPlay()
	{
		// BSTODO: Begin play for all objects
	}

	void FScene::EndPlay()
	{
		// BSTODO: End play for all objects
	}

	void FScene::Tick(float DeltaTime)
	{
		TArray<ISystem*>& Systems = FSystemManager::GetInstance().GetSystems();

		// BSTODO: Fixed time stamp
		{
			for (ISystem* const System : Systems)
			{
				if (System)
				{
					System->FixedTick();
				}
			}
		}

		for (ISystem* const System : Systems)
		{
			if (System)
			{
				System->Tick(DeltaTime);
			}
		}

		for (ISystem* const System : Systems)
		{
			if (System)
			{
				System->LateTick(DeltaTime);
			}
		}
	}
}
