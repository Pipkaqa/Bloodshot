#include "Object/ObjectGlobals.h"

namespace Bloodshot
{
	namespace Private
	{
		FObjectCore& FObjectCore::GetInstance()
		{
			static FObjectCore Instance;
			return Instance;
		}

		void FObjectCore::Dispose()
		{
			for (TPair<const IObject*, FClass*> ObjectClassPair : ObjectClassMappings)
			{
				delete ObjectClassPair.first;
				delete ObjectClassPair.second;
			}

			ObjectClassMappings.clear();
			UniqueIDObjectMappings.clear();
			ObjectFreeSlots.clear();
		}

		IObject* IObjectCoreInterface::FindObjectByUniqueID(const size_t Slot)
		{
			return FObjectCore::GetInstance().UniqueIDObjectMappings[Slot];
		}

		void IObjectConstructor::Destruct(IObject* const Object)
		{
			const size_t Slot = Object->UniqueID;

			FObjectCore& ObjectCore = FObjectCore::GetInstance();

			ObjectCore.ObjectFreeSlots.push_front(Slot);
			ObjectCore.UniqueIDObjectMappings[Slot] = nullptr;

			delete std::exchange(ObjectCore.ObjectClassMappings[Object], nullptr);
			delete Object;
		}
	}

	void DeleteObject(IObject* const Object)
	{
		Private::IObjectConstructor::Destruct(Object);
	}
}
