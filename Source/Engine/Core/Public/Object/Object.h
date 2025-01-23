#pragma once

#include <type_traits>

namespace Bloodshot
{
	class FClass;

	class FObject
	{
	public:
		FObject* GetObject();
	};

	template<typename T>
	concept IsObject = std::is_base_of_v<FObject, T>;
}
