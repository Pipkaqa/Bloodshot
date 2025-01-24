#pragma once

#include "Object/Class.h"
#include "Object/Object.h"
#include "Templates/Containers/String.h"

namespace Bloodshot
{
	class FMirror final
	{
	public:
		template<IsObject T>
		static FClass GetClass();

		template<IsObject T, typename ReturnType>
		static ReturnType& GetPropertyValue(T* const Object, FStringView PropertyName);

		template<IsObject T, typename ReturnType>
		static const ReturnType& GetPropertyValue(const T* const Object, FStringView PropertyName);
	};
}
