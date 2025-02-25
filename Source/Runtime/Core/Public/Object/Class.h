#pragma once

#include "Containers/Array.h"
#include "Containers/StringFwd.h"
#include "Misc/Casts.h"
#include "Object/Object.h"

namespace Bloodshot
{
	namespace Private
	{
		struct IClassConstructor;
	}

	class FProperty final
	{
		friend struct Private::IClassConstructor;

	public:
		FProperty(const char* Type,
			const char* Name,
			const bool bStatic,
			const bool bSerialized,
			const bool bReplicated,
			void* Value);

		~FProperty() {}

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
			return ReinterpretCast<T*>(Value);
		}

	private:
		const char* Type;
		const char* Name;

		const bool bStatic;
		const bool bSerialized;
		const bool bReplicated;

		void* Value = nullptr;
	};

	class FParameter final
	{
		friend struct Private::IClassConstructor;

	public:
		FParameter(const char* Type, const char* Name);
		~FParameter() {}

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
		friend struct Private::IClassConstructor;

	public:
		FFunction(const char* ReturnType,
			const char* Name,
			TArray<FParameter*>&& Parameters,
			const bool bStatic,
			const bool bConst,
			const bool bNoexcept,
			FBoundedFunctionPtr Func);

		~FFunction();

		NODISCARD FORCEINLINE const char* GetReturnType() const
		{
			return ReturnType;
		}

		NODISCARD FORCEINLINE const char* GetName() const
		{
			return Name;
		}

		NODISCARD FORCEINLINE TArray<FParameter*>& GetParameters()
		{
			return Parameters;
		}

		NODISCARD FORCEINLINE bool IsStatic() const
		{
			return bStatic;
		}

		NODISCARD FORCEINLINE bool IsConst() const
		{
			return bConst;
		}

		NODISCARD FORCEINLINE bool IsNoexcept() const
		{
			return bNoexcept;
		}

		void Invoke(IObject* Object, FFunctionParams* Params);

	private:
		const char* ReturnType;
		const char* Name;

		TArray<FParameter*> Parameters;

		const bool bStatic = false;
		const bool bConst = false;
		const bool bNoexcept = false;

		FBoundedFunctionPtr Func;
	};

	class FClass final
	{
		friend struct Private::IClassConstructor;
		friend class IObject;

	public:
		FClass(const char* Name,
			const char* Namespace,
			TArray<FClass*>&& BaseClasses,
			TArray<FProperty*>&& Properties,
			TArray<FFunction*>&& Functions,
			const bool bAbstract,
			const bool bFinal,
			const bool bDerived,
			const size_t Size);

		~FClass();

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

		FClass* FindBaseClass(FStringView Name);
		FProperty* FindProperty(FStringView Name);
		FFunction* FindFunction(FStringView Name);

	private:
		const char* Name;
		const char* Namespace;

		TArray<FClass*> BaseClasses;
		TArray<FProperty*> Properties;
		TArray<FFunction*> Functions;

		const bool bAbstract;
		const bool bFinal;
		const bool bDerived;

		const size_t Size;
	};
}
