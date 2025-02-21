#include "Object/Class.h"
#include "Containers/StringView.h"

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

	FFunction::~FFunction()
	{
		for (FParameter* Parameter : Parameters)
		{
			delete Parameter;
		}
	}

	void FFunction::Invoke(IObject* Object, FFunctionParams* Params)
	{
		Func(Object, Params);
	}

	FClass::FClass(const char* InName,
		const char* InNamespace,
		TArray<FClass*>&& InBaseClasses,
		TArray<FProperty*>&& InProperties,
		TArray<FFunction*>&& InFunctions,
		const bool InbAbstract,
		const bool InbFinal,
		const bool InbDerived,
		const size_t InSize)
		: Name(InName)
		, Namespace(InNamespace)
		, BaseClasses(std::move(InBaseClasses))
		, Properties(std::move(InProperties))
		, Functions(std::move(InFunctions))
		, bAbstract(InbAbstract)
		, bFinal(InbFinal)
		, bDerived(InbDerived)
		, Size(InSize)
	{
	}

	FClass::~FClass()
	{
		for (FClass* BaseClass : BaseClasses)
		{
			delete BaseClass;
		}

		for (FProperty* Property : Properties)
		{
			delete Property;
		}

		for (FFunction* Function : Functions)
		{
			delete Function;
		}
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
