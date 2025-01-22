#pragma once

#include <utility>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename ElementType, typename... ElemenTypes>
	using TPair = std::pair<ElementType, ElemenTypes...>;
}