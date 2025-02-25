#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	FEntityManager& FEntityManager::GetInstance()
	{
		static FEntityManager Instance;
		return Instance;
	}

	TReference<FEntity> FEntityManager::Instantiate()
	{
		BS_PROFILE_FUNCTION();
		TReference<FEntity> Entity = NewObject<FEntity>();
		GetInstance().Entities[Entity->GetUniqueID()] = Entity;
		Entity->TransformComponent = FComponentManager::AddComponent<FTransformComponent>(Entity);
		return Entity;
	}

	FEntityManager::FEntityArray FEntityManager::Instantiate(const size_t Count)
	{
		BS_PROFILE_FUNCTION();
		FEntityArray Result;

		for (size_t i = 0; i < Count; ++i)
		{
			Result.EmplaceBack(Instantiate());
		}

		return Result;
	}

	void FEntityManager::Instantiate(FEntityArray& OutResult, const size_t Count)
	{
		BS_PROFILE_FUNCTION();
		OutResult.Resize(Count);

		for (TReference<FEntity>& Entity : OutResult)
		{
			Entity = Instantiate();
		}
	}

	void FEntityManager::Destroy(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();
		FEntityManager& Instance = GetInstance();
		FComponentManager::RemoveAllComponents(Entity);
		const uint32_t EntityUniqueID = Entity->GetUniqueID();

		if (!Instance.Contains(EntityUniqueID))
		{
			BS_LOG(Error, "Trying to destroy not existing entity");
			return;
		}

		DeleteObject(Entity->GetObject());
		Instance.Entities.at(EntityUniqueID) = nullptr;
	}

	void FEntityManager::Destroy(FEntityArray& OutEntities)
	{
		BS_PROFILE_FUNCTION();

		for (TReference<FEntity>& Entity : OutEntities)
		{
			if (Entity)
			{
				Destroy(Entity);
			}
		}
	}

	void FEntityManager::DestroyAllEntities()
	{
		BS_PROFILE_FUNCTION();

		for (TPair<const size_t, TReference<FEntity>> EntityPair : Entities)
		{
			TReference<FEntity> Entity = EntityPair.second;

			if (Entity)
			{
				Destroy(Entity);
			}
		}

		Entities.clear();
	}

	bool FEntityManager::Contains(const uint32_t EntityUniqueID)
	{
		BS_PROFILE_FUNCTION();

		return Private::Object::FObjectCore::FindObjectByUniqueID(EntityUniqueID);
	}
}
