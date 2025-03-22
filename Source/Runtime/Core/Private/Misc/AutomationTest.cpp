#ifdef BS_WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

namespace Bloodshot
{
	FAutomationTestFramework& FAutomationTestFramework::GetInstance()
	{
		static FAutomationTestFramework Instance;
		return Instance;
	}

	void FAutomationTestFramework::RunTest(FStringView Name)
	{
		IAutomationTest* Test = AutomationTests[Name];
		CurrentTest = Test;
		Test->Run();
		CurrentTest = nullptr;
	}

	void FAutomationTestFramework::RunAllTests()
	{
		for (const TPair<FStringView, IAutomationTest*>& AutomationTestPair : AutomationTests)
		{
			IAutomationTest* Test = AutomationTestPair.Second;
			CurrentTest = Test;
			Test->Run();
		}

		CurrentTest = nullptr;
	}
}

#endif
