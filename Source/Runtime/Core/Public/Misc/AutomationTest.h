#pragma once

#ifdef BS_WITH_AUTOMATION_TESTS

#include "Containers/String.h"
#include "Containers/UnorderedMap.h"
#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	class IAutomationTest;

	class FAutomationTestFramework final
	{
		friend class IAutomationTest;
		friend class IEngineEditorContext;
		friend class IEngineGameContext;

	public:
		NODISCARD static FAutomationTestFramework& GetInstance();
		NODISCARD IAutomationTest* GetCurrentTest();

		void RunTest(const FString& Name);
		void RunAllTests();

	private:
		void RegisterTest(const FString& InName, IAutomationTest* InTest);
		void UnregisterTest(const FString& InName);

		TUnorderedMap<FString, IAutomationTest*> AutomationTests;
		IAutomationTest* CurrentTest = nullptr;
	};

	class IAutomationTest
	{
		friend class FAutomationTestFramework;

	public:
		IAutomationTest(FStringView InName);
		virtual ~IAutomationTest();

		FORCEINLINE void AddError(FStringView ConditionText, FStringView ErrorText)
		{
			BS_LOG(Error, "{}, {} - {}", Name, ConditionText, ErrorText);
		}

	protected:
		FString Name;

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
