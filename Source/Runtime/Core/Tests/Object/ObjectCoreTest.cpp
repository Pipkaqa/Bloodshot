#ifdef BS_WITH_AUTOMATION_TESTS

#include "ObjectCoreTest.h"
#include "Object/ObjectGlobals.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot::Test
{
	TEST_CASE_NAMED(FObjectCoreNewObjectDeleteObjectTest)
	{
		BS_PROFILE_RANGE("FObjectCoreNewObjectDeleteObjectTest");
		IObject* const Object = NewObject<FTestObject>();
		DeleteObject(Object);
	}

	TEST_CASE_NAMED(FObjectCoreNewObjectDeleteObjectStressTest)
	{
		BS_PROFILE_RANGE("FObjectCoreNewObjectDeleteObjectStressTest");
		TArray<IObject*> Objects;
		for (size_t i = 0; i < 100000; ++i)
		{
			Objects.EmplaceBack(NewObject<FTestObject>());
		}
		for (IObject* const Object : Objects)
		{
			DeleteObject(Object);
		}
	}

	TEST_CASE_NAMED(FObjectCoreStaticClassTest)
	{
		BS_PROFILE_RANGE("FObjectCoreGetClassTest");

		IObject* const Object = NewObject<FTestObject>();
		FClass* const TestObjectClass = Object->StaticClass();

		TEST_ASSERT(TestObjectClass->IsAbstract());
		TEST_ASSERT(!TestObjectClass->IsFinal());
		TEST_ASSERT(!TestObjectClass->IsDerived());
		TEST_ASSERT(!TestObjectClass->FindBaseClassByName("IObject"));

		DeleteObject(Object);
	}

	TEST_CASE_NAMED(FObjectCoreGetClassTest)
	{
		BS_PROFILE_RANGE("FObjectCoreGetClassTest");

		IObject* const Object = NewObject<FTestObject>();
		FClass* const TestObjectClass = Object->GetClass();

		TEST_ASSERT(!TestObjectClass->IsAbstract());
		TEST_ASSERT(TestObjectClass->IsFinal());
		TEST_ASSERT(TestObjectClass->IsDerived());
		TEST_ASSERT(TestObjectClass->FindBaseClassByName("IObject"));

		DeleteObject(Object);
	}
}

#endif
