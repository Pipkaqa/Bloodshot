#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable '(UniqueID, TypeID, ObjectClass)' is uninitialized

#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	class IObject;

	template<typename T>
	concept IsObject = std::is_base_of_v<IObject, T>;

	template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes>
	struct IFunctionCaller;

	namespace Private
	{
		class FObjectCore;
	}

	class FClass;

	class IObject
	{
		friend class Private::FObjectCore;

	public:
		IObject() {}
		virtual ~IObject() {}

		inline static FClass* StaticClass()
		{
			return GetPrivateStaticClass();
		};

		NODISCARD FORCEINLINE uint32_t GetUniqueID() const noexcept
		{
			return UniqueID;
		}

		NODISCARD FORCEINLINE uint32_t GetTypeID() const noexcept
		{
			return TypeID;
		}

		NODISCARD FORCEINLINE IObject* GetObject() noexcept
		{
			return this;
		}

		NODISCARD FORCEINLINE FClass* GetClass() noexcept
		{
			return ObjectClass;
		}

		NODISCARD FORCEINLINE bool IsA(FClass* const Class) const noexcept;

	private:
		uint32_t UniqueID;
		uint32_t TypeID;
		FClass* ObjectClass;

		static FClass* GetPrivateStaticClass();
	};
}

#pragma warning(pop)
