#include "EntityManager.h"
#include "ComponentManager.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	FEntityManager::FEntityManager()
	{
		Instance = this;
	}

	void FEntityManager::Init()
	{
		BS_LOG(Debug, "Creating FEntityManager...");
	}

	void FEntityManager::Dispose()
	{
		BS_LOG(Debug, "Destroying FEntityManager...");
	}

	TReference<FEntity> FEntityManager::Instantiate()
	{
		BS_PROFILE_FUNCTION();

		const FInstanceID EntityInstanceID = Reserve();

		TReference<FEntity> Entity = NewObject<FEntity>(EntityInstanceID);

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

	void FEntityManager::InstantiateMultiple(FEntityArray& OutResult, const size_t Count)
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

		FComponentManager::RemoveAllComponents(Entity);

		const FInstanceID EntityInstanceID = Entity->InstanceID;

		if (!Contains(EntityInstanceID))
		{
			BS_LOG(Error, "Trying to destroy not existing entity");
			return;
		}

		BS_LOG(Trace, "Destroying FEntity with InstanceID: {}...", EntityInstanceID.Value);

		DeleteObject(Entity->GetObject());
		Unstore(EntityInstanceID);
	}

	void FEntityManager::DestroyMultiple(FEntityArray& OutEntities)
	{
		BS_PROFILE_FUNCTION();

		for (TReference<FEntity> Entity : OutEntities)
		{
			if (Entity) Destroy(Entity);
		}

		OutEntities.Clear();
	}

	void FEntityManager::DestroyAllEntities()
	{
		BS_PROFILE_FUNCTION();

		for (TReference<FEntity> Entity : Instance->Entities)
		{
			if (Entity) Destroy(Entity);
		}
	}

	FInstanceID FEntityManager::Reserve()
	{
		BS_PROFILE_FUNCTION();

		TList<FInstanceID>& FreeSlotsList = Instance->FreeSlotsList;

		if (!FreeSlotsList.size())
		{
			Resize(Instance->Entities.GetSize() + EntityStorageGrow);
		}

		const FInstanceID EntityInstanceID = FreeSlotsList.front();

		FreeSlotsList.pop_front();

		return EntityInstanceID;
	}

	void FEntityManager::Store(const FInstanceID EntityInstanceID, TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		Instance->Entities[EntityInstanceID] = Entity;
	}

	void FEntityManager::Unstore(const FInstanceID EntityInstanceID)
	{
		BS_PROFILE_FUNCTION();

		Instance->Entities[EntityInstanceID] = nullptr;

		Instance->FreeSlotsList.push_front(EntityInstanceID);
	}

	bool FEntityManager::Contains(const FInstanceID EntityInstanceID)
	{
		BS_PROFILE_FUNCTION();

		const FEntityArray& Entities = Instance->Entities;

		return EntityInstanceID < Entities.GetSize() && Entities[EntityInstanceID];
	}

	void FEntityManager::Resize(const size_t NewSize)
	{
		BS_PROFILE_FUNCTION();

		FEntityArray& Entities = Instance->Entities;

		const size_t OldSize = Entities.GetSize();

		Entities.Resize(NewSize);

		for (size_t i = OldSize; i < NewSize; ++i)
		{
			FInstanceID InstanceID;
			InstanceID.Value = i;
			Instance->FreeSlotsList.push_back(InstanceID);
		}
	}
}
