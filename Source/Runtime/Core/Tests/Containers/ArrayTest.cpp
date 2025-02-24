#ifdef BS_WITH_AUTOMATION_TESTS

#include "Containers/Array.h"
#include "Misc/AutomationTest.h"
#include "Profiling/ProfilingMacros.h"
#include "TestToken.h"

namespace Bloodshot::Test
{
	TEST_CASE_NAMED(FArrayDefaultConstructorTest)
	{
		FTestToken::Reset();
		BS_PROFILE_RANGE("FArrayDefaultConstructorTest");
		TArray<FTestToken> Array;
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!FTestToken::GetConstructionCalls());
	}

	TEST_CASE_NAMED(FArrayCopyConstructorTest)
	{
		BS_PROFILE_RANGE("FArrayCopyConstructorTest");
		TArray<FTestToken> Array = {0, 1, 2, 3, 4};
		FTestToken::Reset();
		TArray<FTestToken> Copied = Array;
		TEST_ASSERT(Array.GetData() != Copied.GetData());
		TEST_ASSERT(Array.GetSize() == Copied.GetSize());
		TEST_ASSERT(Array.GetCapacity() == Copied.GetCapacity());
		TEST_ASSERT(!FTestToken::GetMoveCalls());
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == Copied.GetSize());
		TEST_ASSERT(FTestToken::GetConstructionCalls() == FTestToken::GetCopyConstructorCalls());
	}

	TEST_CASE_NAMED(FArrayMoveConstructorTest)
	{
		BS_PROFILE_RANGE("FArrayMoveConstructorTest");
		TArray<FTestToken> Array = {0, 1, 2, 3, 4};
		FTestToken::Reset();
		TArray<FTestToken> Moved = std::move(Array);
		TEST_ASSERT(Moved.GetSize() == 5);
		TEST_ASSERT(Moved.GetCapacity() == 5);
#pragma warning(disable: 26800)
		TEST_ASSERT(Array.IsEmpty());
#pragma warning(default:26800)
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!Array.GetCapacity());
		TEST_ASSERT(!FTestToken::GetCopyCalls());
		TEST_ASSERT(!FTestToken::GetMoveConstructorCalls());
		TEST_ASSERT(!FTestToken::GetConstructionCalls());
	}

	TEST_CASE_NAMED(FArrayConstructorFromAllocatorTest)
	{
		FTestToken::Reset();
		BS_PROFILE_RANGE("FArrayConstructorFromAllocatorTest");
		TAllocator<FTestToken> Allocator;
		TArray<FTestToken> Array(Allocator);
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!FTestToken::GetConstructionCalls());
	}

	TEST_CASE_NAMED(FArrayConstructorFromInitializerListTest)
	{
		FTestToken::Reset();
		BS_PROFILE_RANGE("FArrayConstructorFromAllocatorTest");
		TArray<FTestToken> Array = {50, 100, 0, 2, 3};
		TEST_ASSERT(Array.GetSize() == 5);
		TEST_ASSERT(Array.GetCapacity() == 5);
		TEST_ASSERT(FTestToken::GetConstructorCalls() == 5);
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == 5);
	}

	TEST_CASE_NAMED(FArrayAssignOfInitializerListWhileEmptyTest)
	{
		FTestToken::Reset();
		BS_PROFILE_RANGE("FArrayAssignOfInitializerListTest");
		TArray<FTestToken> Array;
		Array = {0, 1, 2, 3, 4};
		TEST_ASSERT(FTestToken::GetConstructorCalls() == 5);
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == 5);
	}

	TEST_CASE_NAMED(FArrayAssignOfInitializerListWhileNotEmptyTest)
	{
		BS_PROFILE_RANGE("FArrayAssignOfInitializerListTest");
		TArray<FTestToken> Array = {11, 12, 13, 14, 15, 16, 17, 18};
		FTestToken::Reset();
		Array = {0, 1, 2, 3, 4};
		TEST_ASSERT(FTestToken::GetDestructionCalls() == 8 + 5);
		TEST_ASSERT(FTestToken::GetConstructorCalls() == 5);
		TEST_ASSERT(FTestToken::GetCopyConstructorCalls() == 5);
	}

	TEST_CASE_NAMED(FArrayClearWhileEmptyTest)
	{
		BS_PROFILE_RANGE("FArrayClearWhileEmptyTest");
		TArray<FTestToken> Array;
		TEST_ASSERT(!Array.GetSize());
		Array.Clear();
		TEST_ASSERT(!Array.GetSize());
	}

	TEST_CASE_NAMED(FArrayClearWhileNotEmptyTest)
	{
		BS_PROFILE_RANGE("FArrayClearWhileNotEmptyTest");
		TArray<FTestToken> Array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		FTestToken::Reset();
		Array.Clear();
		TEST_ASSERT(FTestToken::GetDestructionCalls() == 10);
		TEST_ASSERT(Array.GetData());
		TEST_ASSERT(Array.GetCapacity() == 10);
		TEST_ASSERT(Array.GetAllocator().GetAllocatedSize() == sizeof(TArray<FTestToken>::ElementType) * 10);
		TEST_ASSERT(!Array.GetAllocator().GetDeallocatedSize());
	}

	TEST_CASE_NAMED(FArrayDisposeWhileEmptyTest)
	{
		BS_PROFILE_RANGE("FArrayDisposeWhileEmptyTest");
		TArray<FTestToken> Array;
		TEST_ASSERT(!Array.GetData());
		Array.Dispose();
		TEST_ASSERT(!Array.GetData());
	}

	TEST_CASE_NAMED(FArrayDisposeWhileNotEmptyTest)
	{
		BS_PROFILE_RANGE("FArrayDisposeWhileNotEmptyTest");
		TArray<FTestToken> Array = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		FTestToken::Reset();
		Array.Dispose();
		TEST_ASSERT(!Array.GetData());
		TEST_ASSERT(!Array.GetCapacity());
		TEST_ASSERT(!Array.GetSize());
		TEST_ASSERT(FTestToken::GetDestructionCalls() == 10);
	}
}

#endif
