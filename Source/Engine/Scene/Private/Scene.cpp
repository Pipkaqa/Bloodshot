#include "Scene.h"
#include "EntityManager.h"
#include "System.h"
#include "SystemManager.h"

namespace Bloodshot
{
	FScene::FScene(const InstanceID_t InstanceID)
		: InstanceID(InstanceID)
	{
	}

	void FScene::BeginPlay()
	{
		// BSTODO: Begin play for all objects
	}

	void FScene::EndPlay()
	{
		// BSTODO: End play for all objects

		FSystemManager::RemoveAllSystems();
		FEntityManager::DestroyAllEntities();
	}

	void FScene::Tick(float DeltaTime)
	{
		TVector<TUniquePtr<ISystem>>& Systems = FSystemManager::GetSystems();

		// BSTODO: Fixed time stamp
		{
			for (TUniquePtr<ISystem>& System : Systems)
			{
				if (System)
				{
					System->FixedTick();
				}
			}
		}

		for (TUniquePtr<ISystem>& System : Systems)
		{
			if (System)
			{
				System->Tick(DeltaTime);
			}
		}

		for (TUniquePtr<ISystem>& System : Systems)
		{
			if (System)
			{
				System->LateTick(DeltaTime);
			}
		}
	}
}
