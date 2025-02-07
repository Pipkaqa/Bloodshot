#pragma once

#include "Containers/String.h"
#include "Object/Class.h"
#include "Object/Object.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	class FMirror final
	{
	public:
		template<IsObject T>
		static FClass GetClass()
		{
			static_assert(TFalseType<T>::value, "Trying to GetClass() from not reflected class");
		}

		template<IsObject T, typename ReturnType>
		static ReturnType& GetPropertyValue(T* const Object, FStringView PropertyName)
		{
			static_assert(TFalseType<T>::value, "Trying to GetPropertyValue() from not reflected class");
		}

		template<IsObject T, typename ReturnType>
		static const ReturnType& GetPropertyValue(const T* const Object, FStringView PropertyName)
		{
			static_assert(TFalseType<T>::value, "Trying to GetPropertyValue() from not reflected class");
		}
	};
}
