#include "ECS.h"

namespace Bloodshot
{
	void IECS::RemoveAllComponents(IEntity* const Entity)
	{
		BS_PROFILE_FUNCTION();

		FComponentManager::RemoveAllComponents(GetCurrentScene()->ComponentStorage, Entity);
	}

	void IECS::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		FSystemManager::RemoveAllSystems(GetCurrentScene()->SystemStorage);
	}
}
