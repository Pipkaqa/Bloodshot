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

		FTypeIDComponentAllocatorUnorderedMap& ComponentAllocatorsMap = Instance->ComponentAllocatorsMap;

		for (FTypeIDComponentAllocatorUnorderedMap::value_type& AllocatorPair : ComponentAllocatorsMap)
		{
			IComponentAllocator* Allocator = AllocatorPair.second;

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

		IComponentAllocator* const Allocator = It->second;

		BS_ASSERT(It != ComponentAllocatorsMap.end() && Allocator, "Attempting to get not existing EntityPool");

		return Allocator;
	}

	void FComponentManager::RemoveAllComponents(TUniquePtr<FComponentStorage>& Storage, IEntity* const Entity)
	{
		const InstanceID_t EntityInstanceID = Entity->GetUniqueID();

		std::unordered_map<TypeID_t, InstanceID_t>& MappedEntityComponents = Storage->GetMappedEntityComponents(EntityInstanceID);

		for (std::unordered_map<TypeID_t, InstanceID_t>::value_type& ComponentPair : MappedEntityComponents)
		{
			const InstanceID_t ComponentInstanceID = ComponentPair.second;

			if (ComponentInstanceID == InvalidInstanceID) continue;

			IComponent* const Component = Storage->GetComponent(ComponentInstanceID);

			BS_LOG(Trace, "Destroying Component of type: {0}...", Component->GetTypeName());

			Component->EndPlay();

			const TypeID_t ComponentTypeID = ComponentPair.first;

			GetComponentAllocator(ComponentTypeID)->Deallocate(Component);

			Storage->Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}
	}
}
