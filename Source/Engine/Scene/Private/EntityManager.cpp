#include "EntityManager.h"
#include "ComponentManager.h"
#include "Components/TransformComponent.h"

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

		void* const Memory = Instance->EntityAllocator.Allocate(1);

		TReference<FEntity> Entity = new(Memory) FEntity(EntityInstanceID);

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

	void FEntityManager::InstantiateMultiple(FEntityVector& OutResult, const size_t Count)
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

		BS_LOG(Trace, "Destroying Entity with InstanceID: {}...", EntityInstanceID);

		Instance->EntityAllocator.Deallocate(Entity.GetRawPtr(), sizeof(FEntity));

		Unstore(EntityInstanceID);
	}

	void FEntityManager::DestroyMultiple(FEntityVector& OutEntities)
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

		for (TReference<FEntity> Entity : Instance->Entities)
		{
			if (Entity) Destroy(Entity);
		}
	}

	InstanceID_t FEntityManager::Reserve()
	{
		BS_PROFILE_FUNCTION();

		TList<InstanceID_t>& FreeSlotsList = Instance->FreeSlotsList;

		if (!FreeSlotsList.size())
		{
			Resize(Instance->Entities.size() + EntityStorageGrow);
		}

		const InstanceID_t EntityInstanceID = FreeSlotsList.front();

		FreeSlotsList.pop_front();

		return EntityInstanceID;
	}

	void FEntityManager::Store(const InstanceID_t EntityInstanceID, TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		Instance->Entities.at(EntityInstanceID) = Entity;
	}

	void FEntityManager::Unstore(const InstanceID_t EntityInstanceID)
	{
		BS_PROFILE_FUNCTION();

		Instance->Entities.at(EntityInstanceID) = nullptr;

		Instance->FreeSlotsList.push_front(EntityInstanceID);
	}

	bool FEntityManager::Contains(const InstanceID_t EntityInstanceID)
	{
		BS_PROFILE_FUNCTION();

		const FEntityVector& Entities = Instance->Entities;

		return EntityInstanceID < Entities.size() && Entities.at(EntityInstanceID);
	}

	void FEntityManager::Resize(const size_t NewSize)
	{
		BS_PROFILE_FUNCTION();

		FEntityVector& Entities = Instance->Entities;

		const size_t OldSize = Entities.size();

		Entities.resize(NewSize);

		for (size_t i = OldSize; i < NewSize; ++i)
		{
			Instance->FreeSlotsList.push_back(i);
		}
	}
}
