#include "Misc/TypeID.h"

namespace Bloodshot
{
	uint32_t ITypeID::GetTypeCount()
	{
		static uint32_t TypeCount{ 0 };
		return TypeCount++;
	}
}
