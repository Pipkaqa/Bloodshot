#include "Object/Class.h"
#include "Containers/StringView.h"

namespace Bloodshot
{
	FClass* FClass::FindBaseClassByName(FStringView InName)
	{
		for (size_t i = 0; i < BaseClassCount; ++i)
		{
			FClass* const BaseClass = BaseClasses + i;
			if (BaseClass->GetName() == InName)
			{
				return BaseClass;
			}
		}
		return nullptr;
	}

	FProperty* FClass::FindPropertyByName(FStringView InName)
	{
		for (size_t i = 0; i < PropertyCount; ++i)
		{
			FProperty* const Property = Properties + i;
			if (Property->GetName() == InName)
			{
				return Property;
			}
		}
		return nullptr;
	}

	FFunction* FClass::FindFunctionByName(FStringView InName)
	{
		for (size_t i = 0; i < FunctionCount; ++i)
		{
			FFunction* const Function = Functions + i;
			if (Function->GetName() == InName)
			{
				return Function;
			}
		}
		return nullptr;
	}
}
