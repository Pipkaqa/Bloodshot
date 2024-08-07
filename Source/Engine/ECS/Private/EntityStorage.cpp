#include "EntityStorage.h"

#include "ECS.h"
#include "Entity.h"

namespace Bloodshot
{
	FEntityStorage::FEntityStorage(IScene* const Context)
		: Context(Context)
	{
		BS_LOG(Debug, "Creating EntityStorage on Scene of type: {0}...", Context->GetTypeName());

		EntitiesVec.resize(IECS::EntityStorageGrow);
	}

	FEntityStorage::~FEntityStorage()
	{
		BS_LOG(Debug, "Destroying EntityStorage on Scene of type: {0}...", Context->GetTypeName());
	}

	void FEntityStorage::Store(IEntity* const Entity)
	{
		InstanceID_t EntityInstanceID = 0;
		size_t EntitiesVecSize = EntitiesVec.size();

		bool bFreeSpaceFound = false;

		for (; EntityInstanceID < EntitiesVecSize; EntityInstanceID++)
		{
			if (!EntitiesVec[EntityInstanceID])
			{
				bFreeSpaceFound = true;
				break;
			}
		}

		if (!bFreeSpaceFound)
		{
			EntitiesVec.resize(EntitiesVecSize + IECS::EntityStorageGrow);
		}

		EntitiesVec[EntityInstanceID] = Entity;
		Entity->UniqueID = EntityInstanceID;
	}

	void FEntityStorage::Unstore(const InstanceID_t EntityInstanceID)
	{
		EntitiesVec.at(EntityInstanceID) = nullptr;
	}

	void FEntityStorage::Clear()
	{
		EntitiesVec.clear();
	}

	NODISCARD IEntity* FEntityStorage::GetEntity(const InstanceID_t EntityInstanceID)
	{
		return EntitiesVec.at(EntityInstanceID);
	}

	NODISCARD const IEntity* FEntityStorage::GetEntity(const InstanceID_t EntityInstanceID) const
	{
		return EntitiesVec.at(EntityInstanceID);
	}

	NODISCARD FEntityStorage::FEntityVec& FEntityStorage::GetEntityVec()
	{
		return EntitiesVec;
	}

	NODISCARD const FEntityStorage::FEntityVec& FEntityStorage::GetEntityVec() const
	{
		return EntitiesVec;
	}

	NODISCARD bool FEntityStorage::Contains(const InstanceID_t EntityInstanceID) const
	{
		return EntityInstanceID < EntitiesVec.size() && EntitiesVec[EntityInstanceID];
	}
}
