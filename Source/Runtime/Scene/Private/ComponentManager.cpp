#include "ComponentManager.h"

namespace Bloodshot
{
	FComponentManager& FComponentManager::GetInstance()
	{
		static FComponentManager Instance;
		return Instance;
	}

	void FComponentManager::RemoveAllComponents(TReference<FEntity> Entity)
	{
		BS_PROFILE_FUNCTION();

		const uint32_t EntityUniqueID = Entity->GetUniqueID();

		if (!FEntityManager::Contains(EntityUniqueID))
		{
			BS_LOG(Error, "Trying to remove all components from not existing entity");
			return;
		}

		FComponentManager& Instance = GetInstance();
		FEntityComponentTable& EntityComponentTable = Instance.EntityComponentTable;

		for (TPair<const uint32_t, TReference<IComponent>>& ComponentsPair : EntityComponentTable.at(EntityUniqueID))
		{
			TReference<IComponent>& Component = ComponentsPair.second;

			BS_LOG(Trace, "Destroying Component of type: {}...", Component->GetClass()->GetName());

			Component->EndPlay();
			DeleteObject(Component->GetObject());

			Component = nullptr;
		}
	}

	bool FComponentManager::Contains(const uint32_t EntityUniqueID, const uint32_t ComponentTypeID)
	{
		BS_PROFILE_FUNCTION();

		const FEntityComponentTable& EntityComponentTable = GetInstance().EntityComponentTable;

		FEntityComponentTable::const_iterator ComponentsIt = EntityComponentTable.find(EntityUniqueID);

		if (ComponentsIt == EntityComponentTable.end())
		{
			return false;
		}

		TUnorderedMap<uint32_t, TReference<IComponent>>::const_iterator ComponentIt = ComponentsIt->second.find(ComponentTypeID);

		if (ComponentIt == ComponentsIt->second.end())
		{
			return false;
		}

		TReference<IComponent> Component = ComponentIt->second;

		return Component.IsValid();
	}
}
