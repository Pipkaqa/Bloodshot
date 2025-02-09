#include "ComponentManager.h"

namespace Bloodshot
{
	FComponentManager::FComponentManager()
	{
		Instance = this;
	}

	void FComponentManager::Init()
	{
		BS_LOG(Debug, "Creating FComponentManager...");
	}

	void FComponentManager::Dispose()
	{
		BS_LOG(Debug, "Destroying FComponentManager...");

		for (TReference<IComponent> Component : Components)
		{
			DeleteObject(Component->GetObject());
		}
	}

	void FComponentManager::RemoveAllComponents(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		const FInstanceID EntityInstanceID = Entity->InstanceID;

		if (!FEntityManager::Contains(EntityInstanceID))
		{
			BS_LOG(Error, "Trying to remove all Components from not existing Entity");
			return;
		}

		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		if (!EntityComponentTable.GetSize()) return;

		for (const FInstanceID ComponentInstanceID : EntityComponentTable[EntityInstanceID])
		{
			if (!ComponentInstanceID.IsValid()) continue;
			
			TReference<IComponent> Component = Instance->Components[ComponentInstanceID];

			BS_LOG(Trace, "Destroying Component of type: {}...", Component->StaticClass()->GetName());

			const FTypeID ComponentTypeID = Component->TypeID;

			Component->EndPlay();
			DeleteObject(Component->GetObject());
			Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}
	}

	NODISCARD bool FComponentManager::Contains(const FInstanceID EntityInstanceID, const FTypeID ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		const FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		if (EntityInstanceID >= EntityComponentTable.GetSize())
		{
			return false;
		}

		const TArray<FInstanceID>& EntityComponents = EntityComponentTable[EntityInstanceID];

		if (ComponentTypeID >= EntityComponents.GetSize())
		{
			return false;
		}

		const FInstanceID ComponentInstanceID = EntityComponents[ComponentTypeID];

		return ComponentInstanceID.IsValid();
	}

	FInstanceID FComponentManager::Store(TReference<IComponent> Component,
		const FInstanceID EntityInstanceID,
		const FTypeID ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		TList<FInstanceID>& FreeSlotsList = Instance->FreeSlots;

		if (!FreeSlotsList.size())
		{
			ExpandComponentArray();
		}

		const FInstanceID ComponentInstanceID = FreeSlotsList.front();
		FreeSlotsList.pop_front();

		Instance->Components[ComponentInstanceID] = Component;

		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;
		const size_t EntityComponentTableSize = EntityComponentTable.GetSize();

		if (EntityComponentTableSize - 1 < (size_t)EntityInstanceID || !EntityComponentTableSize)
		{
			ExpandEntityComponentTable();
		}

		TArray<FInstanceID>& EntityComponents = EntityComponentTable[EntityInstanceID];
		const uint64_t EntityComponentsSize = EntityComponents.GetSize();

		if (EntityComponentsSize - 1 < ComponentTypeID || !EntityComponentsSize)
		{
			// BSTODO: Temp
			EntityComponents.Resize(EntityComponentsSize + 64);
		}

		EntityComponents[ComponentTypeID] = ComponentInstanceID;
		return ComponentInstanceID;
	}

	void FComponentManager::Unstore(const FInstanceID EntityInstanceID,
		const FInstanceID ComponentInstanceID,
		const FTypeID ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->FreeSlots.push_front(ComponentInstanceID);
		Instance->EntityComponentTable[EntityInstanceID][ComponentTypeID].Reset();
		Instance->Components[ComponentInstanceID] = nullptr;
	}

	void FComponentManager::ExpandEntityComponentTable()
	{
		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		const size_t OldSize = EntityComponentTable.GetSize();
		const size_t NewSize = OldSize + EntityComponentTableExpansion;

		EntityComponentTable.Resize(NewSize);

		for (size_t i = OldSize; i < NewSize; ++i)
		{
			// BSTODO: Temp
			EntityComponentTable[i].Resize(64);
		}
	}

	void FComponentManager::ExpandComponentArray()
	{
		FComponentArray& Components = Instance->Components;

		const size_t OldSize = Components.GetSize();
		const size_t NewSize = OldSize + EntityComponentTableExpansion * 64;

		Components.Resize(NewSize);

		for (size_t i = OldSize; i < NewSize; ++i)
		{
			FInstanceID InstanceID;
			InstanceID.Value = i;
			Instance->FreeSlots.push_back(InstanceID);
		}
	}
}
