#pragma once

#include "Platform/Platform.h"

#include <cstdint>

namespace Bloodshot
{
	class ITypeID final
	{
	public:
		template<typename T>
		NODISCARD static uint32_t Get()
		{
			static uint32_t TypeID{ GetTypeCount() };
			return TypeID;
		}

	private:
		NODISCARD static uint32_t GetTypeCount();
	};
}
