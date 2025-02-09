#include "Object/ObjectGlobals.h"

namespace Bloodshot
{
	namespace Private
	{
		void IObjectConstructor::Destruct(IObject* const Object)
		{
			const size_t Slot = Object->UniqueID;
			GObjectFreeSlots.push_front(Slot);
			GUniqueIDObjectMappings[Slot] = nullptr;
			delete std::exchange(GObjectClassMappings[Object], nullptr);
			delete Object;
		}

		IObject* FindObjectByUniqueID(const size_t Slot)
		{
			return GUniqueIDObjectMappings[Slot];
		}
	}

	void DeleteObject(IObject* const Object)
	{
		Private::IObjectConstructor::Destruct(Object);
	}
}
