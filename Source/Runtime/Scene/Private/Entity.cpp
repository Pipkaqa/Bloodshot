#include "Entity.h"
#include "ComponentManager.h"
#include "EntityManager.h"

namespace Bloodshot
{
	void FEntity::Destroy()
	{
		FEntityManager::Destroy(this);
	}

	void FEntity::RemoveAllComponents()
	{
		FComponentManager::RemoveAllComponents(this);
	}
}
