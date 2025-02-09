#pragma once

#include "Object/ObjectMacros.h"
#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	class FClass;

	namespace Private
	{
		struct IObjectConstructor;
		struct IClassConstructor;
	}

	class IObject;

	template<typename T>
	concept IsObject = std::is_base_of_v<IObject, T>;

	template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes>
	struct IFunctionCaller;

	class IObject
	{
		friend struct Private::IObjectConstructor;

		GENERATED_BODY();

	public:
		IObject();
		virtual ~IObject() {}

		NODISCARD IObject* GetObject();
		NODISCARD FClass* GetClass();

	private:
		size_t UniqueID;
		FClass* ObjectClass;
	};
}
