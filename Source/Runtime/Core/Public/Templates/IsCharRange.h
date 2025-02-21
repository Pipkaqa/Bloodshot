#pragma once

#include "Templates/ElementType.h"
#include "Templates/IsCharType.h"
#include "Templates/IsContiguousContainer.h"

#include <type_traits>

namespace Bloodshot
{
	template<typename T>
	concept IsCharRange = requires
	{
		TIsContiguousContainer_V<T>
			&& !std::is_array_v<std::remove_reference_t<T>>
			&& TIsCharType_V<TElementType_T<T>>;
	};
}
