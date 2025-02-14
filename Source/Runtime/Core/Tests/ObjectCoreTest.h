#pragma once

#ifdef BS_WITH_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#endif

#include "Object/Object.h"
#include "Object/ObjectMacros.h"

namespace Bloodshot::Test
{
	BSCLASS();
	class FTestObject final : public IObject
	{
		GENERATED_BODY();
	};
}
