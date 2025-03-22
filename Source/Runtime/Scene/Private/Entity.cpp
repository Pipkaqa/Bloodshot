#include "Entity.h"
#include "EntityManager.h"

namespace Bloodshot
{
	void FEntity::RemoveAllComponents()
	{
		for (size_t i = 1; i < Components.GetSize(); ++i)
		{
			DestroyComponent(Components[i]);
		}
		Components.Resize(1);
	}

	void FEntity::Destroy()
	{
		FEntityManager::Destroy(this);
	}
}
