#pragma once

#include <tuple>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType, typename... InElemenTypes>
	using TTuple = std::tuple<InElementType, InElemenTypes...>;
}