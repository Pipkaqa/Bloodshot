#include "ComponentStorage.h"

#include "ECS.h"

namespace Bloodshot
{
	FComponentStorage::FComponentStorage(IScene* const Context)
		: Context(Context)
	{
		BS_LOG(Debug, "Creating ComponentStorage on Scene of type: {0}...", Context->GetTypeName());

		ComponentsVec.resize(IECS::ComponentStorageGrow);
	}

	FComponentStorage::~FComponentStorage()
	{
		BS_LOG(Debug, "Destroying ComponentStorage on Scene of type: {0}...", Context->GetTypeName());
	}

	void FComponentStorage::Store(IEntity* const Entity, const InstanceID_t EntityInstanceID, IComponent* const Component, const TypeID_t ComponentTypeID)
	{
		InstanceID_t ComponentInstanceID = 0U;

		const size_t ComponentsVecSize = ComponentsVec.size();

		bool bFreeSpaceFound = false;

		for (; ComponentInstanceID < ComponentsVecSize; ++ComponentInstanceID)
		{
			if (!ComponentsVec[ComponentInstanceID])
			{
				bFreeSpaceFound = true;
				break;
			}
		}

		if (!bFreeSpaceFound)
		{
			ComponentsVec.resize(ComponentsVecSize + IECS::ComponentStorageGrow);
		}

		EntityComponentsTable[EntityInstanceID][ComponentTypeID] = ComponentInstanceID;
		ComponentsVec[ComponentInstanceID] = Component;

		Component->UniqueID = ComponentInstanceID;
		Component->Owner = Entity;
	}

	void FComponentStorage::Unstore(const InstanceID_t EntityInstanceID, const InstanceID_t ComponentInstanceID, const TypeID_t ComponentTypeID)
	{
		EntityComponentsTable[EntityInstanceID][ComponentTypeID] = InvalidInstanceID;
		ComponentsVec[ComponentInstanceID] = nullptr;
	}

	void FComponentStorage::Clear()
	{
		EntityComponentsTable.clear();
		ComponentsVec.clear();
	}

	NODISCARD InstanceID_t FComponentStorage::GetComponentInstanceID(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID) const
	{
		return EntityComponentsTable.at(EntityInstanceID).at(ComponentTypeID);
	}

	NODISCARD IComponent* FComponentStorage::GetComponent(const InstanceID_t ComponentInstanceID)
	{
		return ComponentsVec.at(ComponentInstanceID);
	}

	NODISCARD const IComponent* FComponentStorage::GetComponent(const InstanceID_t ComponentInstanceID) const
	{
		return ComponentsVec.at(ComponentInstanceID);
	}

	NODISCARD IComponent* FComponentStorage::GetComponent(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID)
	{
		return ComponentsVec.at(EntityComponentsTable.at(EntityInstanceID).at(ComponentTypeID));
	}

	NODISCARD const IComponent* FComponentStorage::GetComponent(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID) const
	{
		return ComponentsVec.at(EntityComponentsTable.at(EntityInstanceID).at(ComponentTypeID));
	}

	NODISCARD FComponentStorage::FComponentVec& FComponentStorage::GetComponentVec()
	{
		return ComponentsVec;
	}

	NODISCARD const FComponentStorage::FComponentVec& FComponentStorage::GetComponentVec() const
	{
		return ComponentsVec;
	}

	NODISCARD FComponentStorage::FComponentInstanceIDMap& FComponentStorage::GetMappedEntityComponents(const InstanceID_t EntityInstanceID)
	{
		return EntityComponentsTable[EntityInstanceID];
	}

	NODISCARD const FComponentStorage::FComponentInstanceIDMap& FComponentStorage::GetMappedEntityComponents(const InstanceID_t EntityInstanceID) const
	{
		return EntityComponentsTable.at(EntityInstanceID);
	}

	NODISCARD bool FComponentStorage::Contains(const InstanceID_t ComponentInstanceID) const
	{
		return ComponentInstanceID < ComponentsVec.size() && ComponentsVec[ComponentInstanceID];
	}

	NODISCARD bool FComponentStorage::Contains(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID) const
	{
		FEntityComponentTable::const_iterator MappedComponentsIt = EntityComponentsTable.find(EntityInstanceID);

		if (MappedComponentsIt == EntityComponentsTable.end()) return false;

		const FComponentInstanceIDMap& MappedComponents = MappedComponentsIt->second;

		FComponentInstanceIDMap::const_iterator MappedComponentIt = MappedComponents.find(ComponentTypeID);

		if (MappedComponentIt == MappedComponents.end()) return false;

		if (MappedComponentIt->second == InvalidInstanceID) return false;

		return true;
	}
}
