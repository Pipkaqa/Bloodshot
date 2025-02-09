#include "Entity.h"
#include "ComponentManager.h"
#include "EntityManager.h"

namespace Bloodshot
{
	FEntity::FEntity(const FInstanceID InstanceID)
		: InstanceID(InstanceID)
	{
	}

	void FEntity::Destroy()
	{
		FEntityManager::Destroy(this);
	}

	void FEntity::RemoveAllComponents()
	{
		FComponentManager::RemoveAllComponents(this);
	}
}
