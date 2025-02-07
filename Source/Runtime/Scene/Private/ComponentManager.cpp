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

		FAllocatorMap& Allocators = Instance->Allocators;

		for (TPair<const TypeID_t, TUniquePtr<IComponentAllocator>>& AllocatorPair : Allocators)
		{
			TUniquePtr<IComponentAllocator>& Allocator = AllocatorPair.second;

			if (!Allocator) continue;

			BS_LOG(Trace, "Destroying FComponentPool of undefined type...");

			Allocator.Reset();
		}

		Allocators.clear();
	}

	TReference<FComponentManager::IComponentAllocator> FComponentManager::GetComponentAllocator(const TypeID_t ComponentTypeID)
	{
		FAllocatorMap& Allocators = Instance->Allocators;
		FAllocatorMap::iterator It = Allocators.find(ComponentTypeID);

		BS_ASSERT(It != Allocators.end() && It->second, "Attempting to get not existing EntityPool");

		return It->second.GetReference();
	}

	void FComponentManager::RemoveAllComponents(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		const InstanceID_t EntityInstanceID = Entity->InstanceID;

		if (!FEntityManager::Contains(EntityInstanceID))
		{
			BS_LOG(Error, "Trying to remove all Components from not existing Entity");
			return;
		}

		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		if (!EntityComponentTable.GetSize()) return;

		for (const InstanceID_t ComponentInstanceID : EntityComponentTable[EntityInstanceID])
		{
			if (ComponentInstanceID == InvalidInstanceID) continue;

			TReference<IComponent> Component = Instance->Components[ComponentInstanceID];
			const FComponentInfo& ComponentInfo = Component->Info;

			BS_LOG(Trace, "Destroying Component of type: {}...", ComponentInfo.TypeName);

			Component->EndPlay();
			Component->~IComponent();

			const TypeID_t ComponentTypeID = Component->TypeID;

			GetComponentAllocator(ComponentTypeID)->Deallocate(Component.GetRawPtr(), ComponentInfo.Size);
			Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}
	}

	NODISCARD bool FComponentManager::Contains(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		const FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		if (EntityInstanceID >= EntityComponentTable.GetSize())
		{
			return false;
		}

		const TArray<InstanceID_t>& EntityComponents = EntityComponentTable[EntityInstanceID];

		if (ComponentTypeID >= EntityComponents.GetSize())
		{
			return false;
		}

		const InstanceID_t ComponentInstanceID = EntityComponents[ComponentTypeID];

		return ComponentInstanceID != InvalidInstanceID;
	}

	InstanceID_t FComponentManager::Store(TReference<IComponent> Component,
		const InstanceID_t EntityInstanceID,
		const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		TList<InstanceID_t>& FreeSlotsList = Instance->FreeSlotsList;

		if (!FreeSlotsList.size())
		{
			ExpandComponentVector();
		}

		const InstanceID_t ComponentInstanceID = FreeSlotsList.front();
		FreeSlotsList.pop_front();

		Instance->Components[ComponentInstanceID] = Component;

		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;
		const size_t EntityComponentTableSize = EntityComponentTable.GetSize();

		if (EntityComponentTableSize - 1 < (size_t)EntityInstanceID || !EntityComponentTableSize)
		{
			ExpandEntityComponentTable();
		}

		TArray<InstanceID_t>& EntityComponents = EntityComponentTable[EntityInstanceID];
		const uint64_t EntityComponentsSize = EntityComponents.GetSize();

		if (EntityComponentsSize - 1 < ComponentTypeID || !EntityComponentsSize)
		{
			EntityComponents.Resize(EntityComponentsSize + Instance->Allocators.size());
			for (size_t i = EntityComponentsSize; i < EntityComponents.GetSize(); ++i)
			{
				EntityComponents[i] = InvalidInstanceID;
			}
		}

		EntityComponents[ComponentTypeID] = ComponentInstanceID;
		return ComponentInstanceID;
	}

	void FComponentManager::Unstore(const InstanceID_t EntityInstanceID,
		const InstanceID_t ComponentInstanceID,
		const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->FreeSlotsList.push_front(ComponentInstanceID);
		Instance->EntityComponentTable[EntityInstanceID][ComponentTypeID] = InvalidInstanceID;
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
			EntityComponentTable[i].Resize(Instance->Allocators.size());

			for (InstanceID_t& InstanceID : EntityComponentTable[i])
			{
				InstanceID = InvalidInstanceID;
			}
		}
	}

	void FComponentManager::ExpandComponentVector()
	{
		FComponentVector& Components = Instance->Components;

		const size_t OldSize = Components.GetSize();
		const size_t NewSize = OldSize + EntityComponentTableExpansion * Instance->Allocators.size();

		Components.Resize(NewSize);

		for (size_t i = OldSize; i < NewSize; ++i)
		{
			Instance->FreeSlotsList.push_back(i);
		}
	}
}
