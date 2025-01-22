#pragma once

#include <tuple>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename ElementType, typename... ElemenTypes>
	using TTuple = std::tuple<ElementType, ElemenTypes...>;
}