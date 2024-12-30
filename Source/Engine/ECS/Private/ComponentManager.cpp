#include "ComponentManager.h"
#include "ECS.h"

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

		FTypeIDComponentAllocatorUnorderedMap& ComponentAllocatorsMap = Instance->ComponentAllocatorsMap;

		for (FTypeIDComponentAllocatorUnorderedMap::value_type& AllocatorPair : ComponentAllocatorsMap)
		{
			TReference<IComponentAllocator> Allocator = AllocatorPair.second;

			if (!Allocator) continue;

			BS_LOG(Trace, "Destroying ComponentPool of undefined type...");

			delete Allocator;
		}

		ComponentAllocatorsMap.clear();
	}

	FComponentManager::IComponentAllocator* FComponentManager::GetComponentAllocator(const TypeID_t ComponentTypeID)
	{
		FTypeIDComponentAllocatorUnorderedMap& ComponentAllocatorsMap = Instance->ComponentAllocatorsMap;

		FTypeIDComponentAllocatorUnorderedMap::const_iterator It = ComponentAllocatorsMap.find(ComponentTypeID);

		BS_ASSERT(It != ComponentAllocatorsMap.end() && It->second, "Attempting to get not existing EntityPool");

		return It->second;
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

		std::vector<std::vector<InstanceID_t>>& EntityComponentsTable = Instance->EntityComponentsTable;

		if (!EntityComponentsTable.size()) return;

		for (const InstanceID_t ComponentInstanceID : EntityComponentsTable[EntityInstanceID])
		{
			if (ComponentInstanceID == InvalidInstanceID) continue;

			TReference<IComponent> Component = Instance->ComponentsVec[ComponentInstanceID];

			const FComponentInfo& ComponentInfo = Component->Info;

			BS_LOG(Trace, "Destroying Component of type: {0}...", ComponentInfo.TypeName);

			Component->EndPlay();

			Component->~IComponent();

			const TypeID_t ComponentTypeID = Component->TypeID;

			GetComponentAllocator(ComponentTypeID)->Deallocate(Component, ComponentInfo.Size);

			Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}
	}

	NODISCARD bool FComponentManager::Contains(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		const std::vector<std::vector<InstanceID_t>>& EntityComponentsTable = Instance->EntityComponentsTable;

		return EntityInstanceID < EntityComponentsTable.size() 
			&& ComponentTypeID < EntityComponentsTable[EntityInstanceID].size() 
			&& EntityComponentsTable[EntityInstanceID][ComponentTypeID] != InvalidInstanceID;
	}

	InstanceID_t FComponentManager::Store(TReference<IComponent> Component, 
		const InstanceID_t EntityInstanceID, 
		const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		std::vector<TReference<IComponent>>& ComponentsVec = Instance->ComponentsVec;

		std::list<InstanceID_t>& FreeSlotsList = Instance->FreeSlotsList;

		if (!FreeSlotsList.size())
		{
			Resize(ComponentsVec.size() + IECS::ComponentStorageGrow);
		}

		const InstanceID_t ComponentInstanceID = FreeSlotsList.front();

		FreeSlotsList.pop_front();

		ComponentsVec[ComponentInstanceID] = Component;

		std::vector<std::vector<InstanceID_t>>& EntityComponentsTable = Instance->EntityComponentsTable;

		const uint64_t EntityComponentsTableOldSize = EntityComponentsTable.size();

		// BSTODO: Optimize

		if (EntityComponentsTableOldSize - 1 < EntityInstanceID || !EntityComponentsTableOldSize)
		{
			const size_t NewSize = EntityComponentsTableOldSize + IECS::ComponentStorageGrow;

			EntityComponentsTable.resize(NewSize);

			for (size_t i = EntityComponentsTableOldSize; i < NewSize; i++)
			{
				EntityComponentsTable[i].resize(Instance->ComponentAllocatorsMap.size(), InvalidInstanceID);
			}
		}

		std::vector<InstanceID_t>& EntityComponentsVec = EntityComponentsTable[EntityInstanceID];

		const uint64_t EntityComponentsVecOldSize = EntityComponentsVec.size();

		// BSTODO: Temp hardcoded: + 32

		if (EntityComponentsVecOldSize - 1 < ComponentTypeID || !EntityComponentsVecOldSize)
		{
			EntityComponentsVec.resize(EntityComponentsVecOldSize + 32, InvalidInstanceID);
		}

		EntityComponentsTable[EntityInstanceID][ComponentTypeID] = ComponentInstanceID;

		return ComponentInstanceID;
	}

	void FComponentManager::Unstore(const InstanceID_t EntityInstanceID, 
		const InstanceID_t ComponentInstanceID, 
		const TypeID_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->FreeSlotsList.push_front(ComponentInstanceID);

		Instance->EntityComponentsTable[EntityInstanceID][ComponentTypeID] = InvalidInstanceID;
		Instance->ComponentsVec[ComponentInstanceID] = nullptr;
	}

	void FComponentManager::Resize(const size_t NewSize)
	{
		std::vector<TReference<IComponent>>& ComponentsVec = Instance->ComponentsVec;

		const size_t EntityVecSize = ComponentsVec.size();

		ComponentsVec.resize(NewSize);

		for (size_t i = EntityVecSize; i < ComponentsVec.size(); ++i)
		{
			Instance->FreeSlotsList.push_back(i);
		}
	}
}
