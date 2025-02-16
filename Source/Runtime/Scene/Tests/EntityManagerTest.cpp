#ifdef BS_WITH_AUTOMATION_TESTS

#include "Core.h"

#include "EntityManager.h"

namespace Bloodshot
{
	TEST_CASE_NAMED(FEntityManagerInstantiateDestroyTest)
	{
		BS_PROFILE_RANGE("FEntityManagerInstantiateDestroyTest");

		FLogger::GetInstance().SetLogLevelFlags(~ELogLevel::Trace);

		{
			TReference<FEntity> Entity = FEntityManager::Instantiate();

			TEST_ASSERT(Entity.IsValid());
			TEST_ASSERT(Entity->bActive);

			FEntityManager::Destroy(Entity);
		}

		{
			TArray<TReference<FEntity>> Entities = FEntityManager::Instantiate(100);

			TEST_ASSERT(Entities.GetSize() == 100);

			TReference<FEntity> Entity = Entities[99];

			TEST_ASSERT(Entity.IsValid());
			TEST_ASSERT(Entity->bActive);

			FEntityManager::Destroy(Entities);
		}

		FLogger::GetInstance().SetLogLevelFlags(ELogLevel::All);
	}

	TEST_CASE_NAMED(FEntityManagerInstantiateDestroyStressTest)
	{
		BS_PROFILE_RANGE("FEntityManagerInstantiateDestroyStressTest");

		FLogger::GetInstance().SetLogLevelFlags(~ELogLevel::Trace);

		TArray<TReference<FEntity>> Entities = FEntityManager::Instantiate(100000);

		TEST_ASSERT(Entities.GetSize() == 100000);

		TReference<FEntity> Entity = Entities[99999];

		TEST_ASSERT(Entity.IsValid());
		TEST_ASSERT(Entity->bActive);

		FEntityManager::Destroy(Entities);

		FLogger::GetInstance().SetLogLevelFlags(ELogLevel::All);
	}
}

#endif