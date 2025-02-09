#ifdef BS_WITH_AUTOMATION_TESTS

#include "Core.h"

#include "EntityManager.h"

namespace Bloodshot
{
	TEST_CASE_NAMED(FEntityManagerInstantiateTest)
	{
		TReference<FEntity> Entity = FEntityManager::Instantiate();
		
		TEST_ASSERT(Entity.IsValid());
		TEST_ASSERT(Entity->bActive);
	}

	TEST_CASE_NAMED(FEntityManagerDestroyTest)
	{
		TReference<FEntity> Entity = FEntityManager::Instantiate();
		FEntityManager::Destroy(Entity);
	}
}

#endif