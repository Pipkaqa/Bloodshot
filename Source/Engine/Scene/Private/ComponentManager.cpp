#include "ComponentManager.h"

namespace Bloodshot
{
	FComponentManager::FComponentManager()
	{
		Instance = this;
	}

	void FComponentManager::Init()
	{
		BS_LOG(Debug, "Creating ComponentManager...");
	}

	void FComponentManager::Dispose()
	{
		BS_LOG(Debug, "Destroying ComponentManager...");

		FAllocatorMap& Allocators = Instance->Allocators;

		for (TPair<const TypeID_t, TUniquePtr<IComponentAllocator>>& AllocatorPair : Allocators)
		{
			TUniquePtr<IComponentAllocator>& Allocator = AllocatorPair.second;

			if (!Allocator) continue;

			BS_LOG(Trace, "Destroying ComponentPool of undefined type...");

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
			BS_LOG(Error, "Attempting to remove all Components from not existing Entity");
			return;
		}

		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		if (!EntityComponentTable.size()) return;

		for (const InstanceID_t ComponentInstanceID : EntityComponentTable[EntityInstanceID])
		{
			if (ComponentInstanceID == InvalidInstanceID) continue;

			TReference<IComponent> Component = Instance->Components.at(ComponentInstanceID);
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

		if (EntityInstanceID >= EntityComponentTable.size())
		{
			return false;
		}

		const TVector<InstanceID_t>& EntityComponents = EntityComponentTable.at(EntityInstanceID);

		if (ComponentTypeID >= EntityComponents.size())
		{
			return false;
		}

		const InstanceID_t ComponentInstanceID = EntityComponents.at(ComponentTypeID);

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

		Instance->Components.at(ComponentInstanceID) = Component;

		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;
		const size_t EntityComponentTableSize = EntityComponentTable.size();

		if (EntityComponentTableSize - 1 < (size_t)EntityInstanceID || !EntityComponentTableSize)
		{
			ExpandEntityComponentTable();
		}

		TVector<InstanceID_t>& EntityComponents = EntityComponentTable.at(EntityInstanceID);
		const uint64_t EntityComponentsSize = EntityComponents.size();

		if (EntityComponentsSize - 1 < ComponentTypeID || !EntityComponentsSize)
		{
			EntityComponents.resize(EntityComponentsSize + Instance->Allocators.size(), InvalidInstanceID);
		}

		EntityComponents.at(ComponentTypeID) = ComponentInstanceID;
		return ComponentInstanceID;
	}

	void FComponentManager::Unstore(const InstanceID_t EntityInstanceID,
		const InstanceID_t ComponentInstanceID,
		const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->FreeSlotsList.push_front(ComponentInstanceID);
		Instance->EntityComponentTable.at(EntityInstanceID).at(ComponentTypeID) = InvalidInstanceID;
		Instance->Components.at(ComponentInstanceID) = nullptr;
	}

	void FComponentManager::ExpandEntityComponentTable()
	{
		FEntityComponentTable& EntityComponentTable = Instance->EntityComponentTable;

		const size_t OldSize = EntityComponentTable.size();
		const size_t NewSize = OldSize + EntityComponentTableExpansion;

		EntityComponentTable.resize(NewSize);

		for (TVector<InstanceID_t>& EntityComponents : EntityComponentTable)
		{
			EntityComponents.resize(Instance->Allocators.size(), InvalidInstanceID);
		}
	}

	void FComponentManager::ExpandComponentVector()
	{
		FComponentVector& Components = Instance->Components;

		const size_t OldSize = Components.size();
		const size_t NewSize = OldSize + EntityComponentTableExpansion * Instance->Allocators.size();

		Components.resize(NewSize);

		for (size_t i = OldSize; i < NewSize; ++i)
		{
			Instance->FreeSlotsList.push_back(i);
		}
	}
}
