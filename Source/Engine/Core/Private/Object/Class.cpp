#include "Object/Class.h"

namespace Bloodshot
{
	size_t FClass::GetHash() const
	{
		return std::hash<FString>()(Name);
	}
}
