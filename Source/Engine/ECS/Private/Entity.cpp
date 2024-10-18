#include "Entity.h"
#include "ECS.h"

namespace Bloodshot
{
	FEntity::FEntity(InstanceID_t InstanceID)
		: InstanceID(InstanceID)
	{
	}

	void FEntity::Destroy()
	{
		IECS::Destroy(this);
	}

	void FEntity::RemoveAllComponents()
	{
		IECS::RemoveAllComponents(this);
	}
}
