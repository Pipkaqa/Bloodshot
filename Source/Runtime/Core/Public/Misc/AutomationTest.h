#pragma once

#ifdef BS_WITH_AUTOMATION_TESTS

#include "Containers/Map.h"
#include "Containers/StringView.h"
#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	class IAutomationTest;

	class FAutomationTestFramework final
	{
		friend class IAutomationTest;

	public:
		FORCEINLINE ~FAutomationTestFramework() {}

		NODISCARD static FAutomationTestFramework& GetInstance();

		NODISCARD FORCEINLINE IAutomationTest* GetCurrentTest()
		{
			return CurrentTest;
		}

		void RunTest(FStringView Name);
		void RunAllTests();

	private:
		FORCEINLINE FAutomationTestFramework() {}

		FORCEINLINE void RegisterTest(FStringView InName, IAutomationTest* const InTest)
		{
			AutomationTests.Emplace(InName, InTest);
		}

		FORCEINLINE void UnregisterTest(FStringView InName)
		{
			AutomationTests[InName] = nullptr;
		}

		TMap<FStringView, IAutomationTest*> AutomationTests;
		IAutomationTest* CurrentTest = nullptr;
	};

	class IAutomationTest
	{
		friend class FAutomationTestFramework;

	public:
		IAutomationTest(FStringView InName)
			: Name(InName.GetData())
		{
			FAutomationTestFramework::GetInstance().RegisterTest(Name, this);
		}

		virtual ~IAutomationTest()
		{
			FAutomationTestFramework::GetInstance().UnregisterTest(Name);
		}

		FORCEINLINE void AddError(FStringView ConditionText, FStringView ErrorText)
		{
			BS_LOG(Error, "{}, {} - {}", Name, ConditionText, ErrorText);
		}

	protected:
		FStringView Name;

		virtual void Run() = 0;
	};
}

#define IMPLEMENT_AUTOMATION_TEST(Class, StrName) \
class Class final : public IAutomationTest \
{ \
	public: Class(FStringView InName) : IAutomationTest(InName) {}\
	private: virtual void Run() override { Body(); } \
	void Body(); \
}; \
namespace \
{ \
	Class Class##Instance(StrName); \
} \
void Class::Body()

#define TEST_CASE_NAMED(ClassName) IMPLEMENT_AUTOMATION_TEST(ClassName, #ClassName)

#define TEST_ASSERT(Expression) if(!(Expression)) { FAutomationTestFramework::GetInstance().GetCurrentTest()->AddError(#Expression, "Condition failed"); }

#endif
