#include "Scene.h"
#include "EntityManager.h"
#include "System.h"
#include "SystemManager.h"

namespace Bloodshot
{
	void FScene::BeginPlay()
	{
		// BSTODO: Begin play for all objects
	}

	void FScene::EndPlay()
	{
		// BSTODO: End play for all objects

		FSystemManager::RemoveAllSystems();
		FEntityManager::GetInstance().DestroyAllEntities();
	}

	void FScene::Tick(float DeltaTime)
	{
		TArray<TReference<ISystem>>& Systems = FSystemManager::GetInstance().GetSystems();

		// BSTODO: Fixed time stamp
		{
			for (TReference<ISystem> System : Systems)
			{
				if (System)
				{
					System->FixedTick();
				}
			}
		}

		for (TReference<ISystem> System : Systems)
		{
			if (System)
			{
				System->Tick(DeltaTime);
			}
		}

		for (TReference<ISystem> System : Systems)
		{
			if (System)
			{
				System->LateTick(DeltaTime);
			}
		}
	}
}
