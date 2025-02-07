#pragma once

#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	struct FObjectAllocationInfo final
	{
		size_t Size;
		const char* RealTypeName;
	};

	class IObject
	{
	public:
		IObject* GetObject();

	private:
		FObjectAllocationInfo AllocationInfo;
	};

	template<typename T>
	concept IsObject = std::is_base_of_v<IObject, T>;
}
