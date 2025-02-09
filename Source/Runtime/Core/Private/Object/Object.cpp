#include "Object/Object.h"
#include "Object/ObjectGlobals.h"

namespace Bloodshot
{
	IObject::IObject()
	{
	}

	IObject* IObject::GetObject()
	{
		return this;
	}

	FClass* IObject::GetClass()
	{
		return ObjectClass;
	}
}
