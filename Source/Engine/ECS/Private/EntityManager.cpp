#include "EntityManager.h"
#include "ComponentManager.h"
#include "Logging/LoggingMacros.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	FEntityManager::FEntityManager()
		// BSTODO: Temp
		: EntityAllocator(1024, 128)
	{
		Instance = this;
	}

	void FEntityManager::Init()
	{
		BS_LOG(Debug, "Creating EntityManager...");
	}

	void FEntityManager::Dispose()
	{
		BS_LOG(Debug, "Destroying EntityManager...");
	}

	TReference<FEntity> FEntityManager::Instantiate()
	{
		BS_PROFILE_FUNCTION();

		const InstanceID_t EntityInstanceID = Reserve();

		FEntity* const Entity = new(Instance->EntityAllocator.Allocate(1)) FEntity(EntityInstanceID);

		Store(EntityInstanceID, Entity);

		Entity->TransformComponent = FComponentManager::AddComponent<FTransformComponent>(Entity);

		return Entity;
	}

	void FEntityManager::InstantiateMultiple(const size_t Count)
	{
		BS_PROFILE_FUNCTION();

		for (size_t i = 0; i < Count; ++i)
		{
			Instantiate();
		}
	}

	void FEntityManager::InstantiateMultiple(std::vector<TReference<FEntity>>& OutResult, const size_t Count)
	{
		BS_PROFILE_FUNCTION();

		OutResult.resize(Count);

		for (TReference<FEntity>& Entity : OutResult)
		{
			Entity = Instantiate();
		}
	}

	void FEntityManager::Destroy(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		FComponentManager::RemoveAllComponents(Entity);

		const InstanceID_t EntityInstanceID = Entity->InstanceID;

		if (!Contains(EntityInstanceID))
		{
			BS_LOG(Error, "Attempting to destroy not existing Entity");
			return;
		}

		BS_LOG(Trace, "Destroying Entity with Instance ID: {0}...", EntityInstanceID);

		Instance->EntityAllocator.Deallocate(Entity, sizeof(FEntity));

		Unstore(EntityInstanceID);
	}

	void FEntityManager::DestroyMultiple(std::vector<TReference<FEntity>>& OutEntities)
	{
		BS_PROFILE_FUNCTION();

		for (TReference<FEntity> Entity : OutEntities)
		{
			if (Entity) Destroy(Entity);
		}

		OutEntities.clear();
	}

	void FEntityManager::DestroyAllEntities()
	{
		BS_PROFILE_FUNCTION();

		for (TReference<FEntity> Entity : Instance->EntityVec)
		{
			if (Entity) Destroy(Entity);
		}
	}

	InstanceID_t FEntityManager::Reserve()
	{
		BS_PROFILE_FUNCTION();

		std::list<InstanceID_t>& FreeSlotsList = Instance->FreeSlotsList;

		if (!FreeSlotsList.size())
		{
			Resize(Instance->EntityVec.size() + EntityStorageGrow);
		}

		const InstanceID_t EntityInstanceID = FreeSlotsList.front();

		FreeSlotsList.pop_front();

		return EntityInstanceID;
	}

	void FEntityManager::Store(const InstanceID_t EntityInstanceID, TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		Instance->EntityVec[EntityInstanceID] = Entity;
	}

	void FEntityManager::Unstore(const InstanceID_t EntityInstanceID)
	{
		BS_PROFILE_FUNCTION();

		Instance->EntityVec[EntityInstanceID] = nullptr;

		Instance->FreeSlotsList.push_front(EntityInstanceID);
	}

	bool FEntityManager::Contains(const InstanceID_t EntityInstanceID)
	{
		BS_PROFILE_FUNCTION();

		const std::vector<TReference<FEntity>>& EntityVec = Instance->EntityVec;

		return EntityInstanceID < Instance->EntityVec.size() && EntityVec[EntityInstanceID];
	}

	void FEntityManager::Resize(const size_t NewSize)
	{
		BS_PROFILE_FUNCTION();

		std::vector<TReference<FEntity>>& EntityVec = Instance->EntityVec;

		const size_t EntityVecSize = EntityVec.size();

		EntityVec.resize(NewSize);

		for (size_t i = EntityVecSize; i < EntityVec.size(); ++i)
		{
			Instance->FreeSlotsList.push_back(i);
		}
	}
}
