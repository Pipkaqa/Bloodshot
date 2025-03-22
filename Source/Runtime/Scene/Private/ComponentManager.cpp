#include "ComponentManager.h"

namespace Bloodshot
{
	FComponentManager& FComponentManager::GetInstance()
	{
		static FComponentManager Instance;
		return Instance;
	}
}
