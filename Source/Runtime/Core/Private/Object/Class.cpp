#include "Object/Class.h"

namespace Bloodshot
{
	FProperty::FProperty(const char* Type,
		const char* Name,
		const bool bStatic,
		const bool bSerialized,
		const bool bReplicated,
		void* Value)
		: Type(Type)
		, Name(Name)
		, bStatic(bStatic)
		, bSerialized(bSerialized)
		, bReplicated(bReplicated)
		, Value(Value)
	{
	}

	FParameter::FParameter(const char* Type, const char* Name)
		: Type(Type)
		, Name(Name)
	{
	}

	FFunction::FFunction(const char* ReturnType,
		const char* Name,
		TArray<FParameter*>&& Parameters,
		const bool bStatic,
		const bool bConst,
		const bool bNoexcept,
		FBoundedFunctionPtr Func)
		: ReturnType(ReturnType)
		, Name(Name)
		, Parameters(std::move(Parameters))
		, bStatic(bStatic)
		, bConst(bConst)
		, bNoexcept(bNoexcept)
		, Func(Func)
	{
	}

	void FFunction::Invoke(IObject* Object, FFunctionParams* Params)
	{
		Func(Object, Params);
	}

	FClass::FClass(const char* Name,
		TArray<FClass*>&& BaseClasses,
		TArray<FProperty*>&& Properties,
		TArray<FFunction*>&& Functions,
		const bool bAbstract,
		const bool bFinal,
		const bool bDerived,
		const size_t Size)
		: Name(Name)
		, BaseClasses(std::move(BaseClasses))
		, Properties(std::move(Properties))
		, Functions(std::move(Functions))
		, bAbstract(bAbstract)
		, bFinal(bFinal)
		, bDerived(bDerived)
		, Size(Size)
	{
	}

	FClass* FClass::FindBaseClass(FStringView Name)
	{
		for (FClass* BaseClass : BaseClasses)
		{
			if (BaseClass->GetName() == Name)
			{
				return BaseClass;
			}
		}

		return nullptr;
	}

	FProperty* FClass::FindProperty(FStringView Name)
	{
		for (FProperty* Property : Properties)
		{
			if (Property->GetName() == Name)
			{
				return Property;
			}
		}

		return nullptr;
	}

	FFunction* FClass::FindFunction(FStringView Name)
	{
		for (FFunction* Function : Functions)
		{
			if (Function->GetName() == Name)
			{
				return Function;
			}
		}

		return nullptr;
	}
}
