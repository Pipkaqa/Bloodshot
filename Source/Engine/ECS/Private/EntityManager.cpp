#include "EntityManager.h"
#include "ECS.h"
#include "Logging/LoggingMacros.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	FEntityManager::FEntityManager()
		: EntityAllocator({1024, 128})
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

	FEntity* FEntityManager::Instantiate()
	{
		BS_PROFILE_FUNCTION();

		const InstanceID_t EntityInstanceID = Reserve();

		FEntity* const Entity = new(Instance->EntityAllocator.Allocate(1)) FEntity(EntityInstanceID);

		Store(EntityInstanceID, Entity);

		IECS::AddComponent<FTransformComponent>(Entity);

		return ReinterpretCast<FEntity*>(Entity);
	}

	void FEntityManager::Destroy(FEntity* const Entity)
	{
		BS_PROFILE_FUNCTION();

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

	InstanceID_t FEntityManager::Reserve()
	{
		BS_PROFILE_FUNCTION();

		std::list<InstanceID_t>& FreeSlotsList = Instance->FreeSlotsList;

		if (!FreeSlotsList.size())
		{
			Resize(Instance->EntityVec.size() + IECS::EntityStorageGrow);
		}

		const InstanceID_t EntityInstanceID = FreeSlotsList.front();

		FreeSlotsList.pop_front();

		return EntityInstanceID;
	}

	void FEntityManager::Store(const InstanceID_t EntityInstanceID, FEntity* const Entity)
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

		const std::vector<FEntity*>& EntityVec = Instance->EntityVec;

		return EntityInstanceID < Instance->EntityVec.size() && EntityVec[EntityInstanceID];
	}

	void FEntityManager::Resize(const size_t NewSize)
	{
		std::vector<FEntity*>& EntityVec = Instance->EntityVec;

		const size_t EntityVecSize = EntityVec.size();

		EntityVec.resize(NewSize);

		for (size_t i = EntityVecSize; i < EntityVec.size(); ++i)
		{
			Instance->FreeSlotsList.push_back(i);
		}
	}
}
