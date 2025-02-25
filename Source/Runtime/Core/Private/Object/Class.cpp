#include "Object/Class.h"
#include "Containers/StringView.h"

namespace Bloodshot
{
	FProperty::FProperty(const char* InType,
		const char* InName,
		const bool bInStatic,
		const bool bInSerialized,
		const bool bInReplicated,
		void* InValue)
		: Type(InType)
		, Name(InName)
		, bStatic(bInStatic)
		, bSerialized(bInSerialized)
		, bReplicated(bInReplicated)
		, Value(InValue)
	{
	}

	FParameter::FParameter(const char* InType, const char* InName)
		: Type(InType)
		, Name(InName)
	{
	}

	FFunction::FFunction(const char* InReturnType,
		const char* InName,
		TArray<FParameter*>&& InParameters,
		const bool bInStatic,
		const bool bInConst,
		const bool bInNoexcept,
		FBoundedFunctionPtr InFunc)
		: ReturnType(InReturnType)
		, Name(InName)
		, Parameters(std::move(InParameters))
		, bStatic(bInStatic)
		, bConst(bInConst)
		, bNoexcept(bInNoexcept)
		, Func(InFunc)
	{
	}

	FFunction::~FFunction()
	{
		for (FParameter* Parameter : Parameters)
		{
			delete Parameter;
		}
	}

	void FFunction::Invoke(IObject* InObject, FFunctionParams* InParams)
	{
		Func(InObject, InParams);
	}

	FClass::FClass(const char* InName,
		const char* InNamespace,
		TArray<FClass*>&& InBaseClasses,
		TArray<FProperty*>&& InProperties,
		TArray<FFunction*>&& InFunctions,
		const bool InbAbstract,
		const bool InbFinal,
		const bool InbDerived,
		const size_t InSize,
		const uint32_t InTypeID)
		: Name(InName)
		, Namespace(InNamespace)
		, BaseClasses(std::move(InBaseClasses))
		, Properties(std::move(InProperties))
		, Functions(std::move(InFunctions))
		, bAbstract(InbAbstract)
		, bFinal(InbFinal)
		, bDerived(InbDerived)
		, Size(InSize)
		, TypeID(InTypeID)
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

	FClass* FClass::FindBaseClass(FStringView InName)
	{
		for (FClass* BaseClass : BaseClasses)
		{
			if (BaseClass->GetName() == InName)
			{
				return BaseClass;
			}
		}

		return nullptr;
	}

	FProperty* FClass::FindProperty(FStringView InName)
	{
		for (FProperty* Property : Properties)
		{
			if (Property->GetName() == InName)
			{
				return Property;
			}
		}

		return nullptr;
	}

	FFunction* FClass::FindFunction(FStringView InName)
	{
		for (FFunction* Function : Functions)
		{
			if (Function->GetName() == InName)
			{
				return Function;
			}
		}

		return nullptr;
	}
}
