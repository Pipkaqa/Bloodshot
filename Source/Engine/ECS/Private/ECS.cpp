#include "ECS.h"

namespace Bloodshot
{
	FEntity* IECS::Instantiate()
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

	void IECS::InstantiateMultiple(std::vector<FEntity*>& OutResult, const size_t Count)
	{
		BS_PROFILE_FUNCTION();

		OutResult.resize(Count);

		for (FEntity*& Entity : OutResult)
		{
			Entity = FEntityManager::Instantiate();
		}
	}

	void IECS::Destroy(FEntity* const Entity)
	{
		BS_PROFILE_FUNCTION();

		FComponentManager::RemoveAllComponents(Entity);
		FEntityManager::Destroy(Entity);
	}

	void IECS::DestroyMultiple(std::vector<FEntity*>& OutEntities)
	{
		BS_PROFILE_FUNCTION();

		for (FEntity* const Entity : OutEntities)
		{
			if (!Entity) continue;

			FComponentManager::RemoveAllComponents(Entity);
			FEntityManager::Destroy(Entity);
		}

		OutEntities.clear();
	}

	void IECS::RemoveAllComponents(FEntity* const Entity)
	{
		BS_PROFILE_FUNCTION();

		FComponentManager::RemoveAllComponents(Entity);
	}

	void IECS::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		FSystemManager::RemoveAllSystems();
	}
}
