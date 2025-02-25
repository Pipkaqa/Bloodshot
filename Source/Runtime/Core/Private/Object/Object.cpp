#include "Object/Object.h"
#include "Object/Class.h"
#include "Object/ObjectGlobals.h"

namespace Bloodshot
{
	FClass* IObject::GetPrivateStaticClass()
	{
		static FClass Instance("IObject", "::Bloodshot", {}, {}, {}, true, false, false, sizeof(IObject), 0);
		return &Instance;
	}

	bool IObject::IsA(FClass* const Class) const noexcept
	{
		return Class->Name == ObjectClass->Name
			&& Class->Namespace == ObjectClass->Namespace
			&& Class->Size == ObjectClass->Size;
	}

	template<>
	FClass* Private::Object::FObjectCore::ConstructClass<IObject>(IObject* Object)
	{
		return new FClass(*IObject::StaticClass());
	}
}
