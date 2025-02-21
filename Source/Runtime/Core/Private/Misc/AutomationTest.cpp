#ifdef BS_WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Containers/StringView.h"

namespace Bloodshot
{
	FAutomationTestFramework& FAutomationTestFramework::GetInstance()
	{
		static FAutomationTestFramework Instance;
		return Instance;
	}

	void FAutomationTestFramework::RunTest(const FString& Name)
	{
		IAutomationTest* Test = AutomationTests.at(Name);
		CurrentTest = Test;
		Test->Run();
		CurrentTest = nullptr;
	}

	void FAutomationTestFramework::RunAllTests()
	{
		for (const TPair<const FString, IAutomationTest*>& AutomationTestPair : AutomationTests)
		{
			IAutomationTest* Test = AutomationTestPair.second;
			CurrentTest = Test;
			Test->Run();
		}

		CurrentTest = nullptr;
	}

	void FAutomationTestFramework::RegisterTest(const FString& InName, IAutomationTest* InTest)
	{
		AutomationTests.emplace(InName, InTest);
	}

	void FAutomationTestFramework::UnregisterTest(const FString& InName)
	{
		AutomationTests[InName] = nullptr;
	}

	IAutomationTest::IAutomationTest(FStringView InName)
		: Name(InName.GetData())
	{
		FAutomationTestFramework::GetInstance().RegisterTest(Name, this);
	}

	IAutomationTest::~IAutomationTest()
	{
		FAutomationTestFramework::GetInstance().UnregisterTest(Name);
	}
}

#endif
