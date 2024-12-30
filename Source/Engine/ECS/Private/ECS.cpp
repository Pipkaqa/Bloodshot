#include "ECS.h"

namespace Bloodshot
{
	TReference<FEntity> IECS::Instantiate()
	{
		BS_PROFILE_FUNCTION();

		return FEntityManager::Instantiate();
	}

	void IECS::InstantiateMultiple(const size_t Count)
	{
		BS_PROFILE_FUNCTION();

		for (size_t i = 0; i < Count; ++i)
		{
			FEntityManager::Instantiate();
		}
	}

	void IECS::InstantiateMultiple(std::vector<TReference<FEntity>>& OutResult, const size_t Count)
	{
		BS_PROFILE_FUNCTION();

		OutResult.resize(Count);

		for (TReference<FEntity>& Entity : OutResult)
		{
			Entity = FEntityManager::Instantiate();
		}
	}

	void IECS::Destroy(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		if (!Entity) return;

		FComponentManager::RemoveAllComponents(Entity);
		FEntityManager::Destroy(Entity);
	}

	void IECS::DestroyMultiple(std::vector<TReference<FEntity>>& OutEntities)
	{
		BS_PROFILE_FUNCTION();

		for (TReference<FEntity> Entity : OutEntities)
		{
			if (!Entity) continue;

			FComponentManager::RemoveAllComponents(Entity);
			FEntityManager::Destroy(Entity);
		}

		OutEntities.clear();
	}

	void IECS::RemoveAllComponents(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		FComponentManager::RemoveAllComponents(Entity);
	}

	void IECS::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		FSystemManager::RemoveAllSystems();
	}

	void IECS::InternalRemoveAllComponents()
	{
		for (TReference<FEntity> Entity : FEntityManager::Instance->EntityVec)
		{
			if (!Entity) continue;

			FComponentManager::RemoveAllComponents(Entity);
		}
	}

	void IECS::InternalDestroyAllEntities()
	{
		DestroyMultiple(FEntityManager::Instance->EntityVec);
	}
}
