#pragma once

#include "Containers/Array.h"
#include "Containers/StringFwd.h"
#include "Object/Object.h"

namespace Bloodshot
{
	namespace Private::Object
	{
		class FObjectCore;
	}

	class FProperty final
	{
	public:
		FProperty(const char* InType,
			const char* InName,
			const bool bInStatic,
			const bool bInSerialized,
			const bool bInReplicated,
			void* const InValue)
			: Type(InType)
			, Name(InName)
			, bStatic(bInStatic)
			, bSerialized(bInSerialized)
			, bReplicated(bInReplicated)
			, Value(InValue)
		{
		}

		NODISCARD FORCEINLINE const char* GetType() const
		{
			return Type;
		}

		NODISCARD FORCEINLINE const char* GetName() const
		{
			return Name;
		}

		NODISCARD FORCEINLINE bool IsStatic() const
		{
			return bStatic;
		}

		NODISCARD FORCEINLINE bool IsSerialized() const
		{
			return bSerialized;
		}

		NODISCARD FORCEINLINE bool IsReplicated() const
		{
			return bReplicated;
		}

		template<typename T>
		NODISCARD FORCEINLINE T* GetValue()
		{
			return Value ? reinterpret_cast<T*>(Value) : nullptr;
		}

	private:
		const char* Type;
		const char* Name;

		bool bStatic;
		bool bSerialized;
		bool bReplicated;

		void* Value;
	};

	class FParameter final
	{
	public:
		FORCEINLINE FParameter(const char* InType, const char* InName)
			: Type(InType)
			, Name(InName)
		{
		}

		NODISCARD FORCEINLINE const char* GetType() const
		{
			return Type;
		}

		NODISCARD FORCEINLINE const char* GetName() const
		{
			return Name;
		}

	private:
		const char* Type;
		const char* Name;
	};

	struct FFunctionParams final
	{
		FFunctionParams* Next;
		size_t Size;
		void* Value;
	};

	using FBoundedFunctionPtr = void (*)(IObject* Object, FFunctionParams* Params);

	class FFunction final
	{
	public:
		FORCEINLINE FFunction(const char* InReturnType,
			const char* InName,
			FParameter* const InParameters,
			const size_t InParameterCount,
			const bool bInStatic,
			const bool bInConst,
			const bool bInNoexcept,
			FBoundedFunctionPtr InFunc)
			: ReturnType(InReturnType)
			, Name(InName)
			, Parameters(InParameters)
			, ParameterCount(InParameterCount)
			, bStatic(bInStatic)
			, bConst(bInConst)
			, bNoexcept(bInNoexcept)
			, Func(InFunc)
		{
		}

		NODISCARD FORCEINLINE const char* GetReturnType() const
		{
			return ReturnType;
		}

		NODISCARD FORCEINLINE const char* GetName() const
		{
			return Name;
		}

		NODISCARD FORCEINLINE bool IsStatic() const noexcept
		{
			return bStatic;
		}

		NODISCARD FORCEINLINE bool IsConst() const noexcept
		{
			return bConst;
		}

		NODISCARD FORCEINLINE bool IsNoexcept() const noexcept
		{
			return bNoexcept;
		}

		FORCEINLINE void Invoke(IObject* const InObject, FFunctionParams* const InParams) const
		{
			Func(InObject, InParams);
		}

	private:
		const char* ReturnType;
		const char* Name;

		FParameter* Parameters;
		size_t ParameterCount;

		bool bStatic;
		bool bConst;
		bool bNoexcept;

		FBoundedFunctionPtr Func;
	};

	class FClass final
	{
		friend class IObject;
		friend class Private::Object::FObjectCore;

	public:
		FORCEINLINE FClass(const char* InName,
			const char* InNamespace,
			FClass* const InBaseClasses,
			const size_t InBaseClassCount,
			FProperty* const InProperties,
			const size_t InPropertyCount,
			FFunction* const InFunctions,
			const size_t InFunctionCount,
			const bool InbAbstract,
			const bool InbFinal,
			const bool InbDerived,
			const size_t InSize,
			const uint32_t InTypeID)
			: Name(InName)
			, Namespace(InNamespace)
			, BaseClasses(InBaseClasses)
			, BaseClassCount(InBaseClassCount)
			, Properties(InProperties)
			, PropertyCount(InPropertyCount)
			, Functions(InFunctions)
			, FunctionCount(InFunctionCount)
			, bAbstract(InbAbstract)
			, bFinal(InbFinal)
			, bDerived(InbDerived)
			, Size(InSize)
			, TypeID(InTypeID)
		{
		}

		NODISCARD FORCEINLINE const char* GetName()
		{
			return Name;
		}

		NODISCARD FORCEINLINE const char* GetNamespace()
		{
			return Namespace;
		}

		NODISCARD FORCEINLINE bool IsAbstract() const noexcept
		{
			return bAbstract;
		}

		NODISCARD FORCEINLINE bool IsFinal() const noexcept
		{
			return bFinal;
		}

		NODISCARD FORCEINLINE bool IsDerived() const noexcept
		{
			return bDerived;
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Size;
		}

		NODISCARD FORCEINLINE uint32_t GetTypeID() const noexcept
		{
			return TypeID;
		}

		FClass* FindBaseClassByName(FStringView Name);
		FProperty* FindPropertyByName(FStringView Name);
		FFunction* FindFunctionByName(FStringView Name);

	private:
		const char* Name;
		const char* Namespace;

		FClass* BaseClasses;
		size_t BaseClassCount;

		FProperty* Properties;
		size_t PropertyCount;

		FFunction* Functions;
		size_t FunctionCount;

		bool bAbstract;
		bool bFinal;
		bool bDerived;

		size_t Size;
		uint32_t TypeID;
	};
}
