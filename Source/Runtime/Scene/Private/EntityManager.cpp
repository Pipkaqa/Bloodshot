#include "EntityManager.h"

namespace Bloodshot
{
	FEntityManager& FEntityManager::GetInstance()
	{
		static FEntityManager Instance;
		return Instance;
	}
}
