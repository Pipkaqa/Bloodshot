#include "Misc/AssertionMacros.h"
#include "Object/ObjectGlobals.h"

namespace Bloodshot
{
	namespace Private
	{
		FObjectCore::~FObjectCore()
		{
			Dispose();
		}

		FObjectCore& FObjectCore::GetInstance()
		{
			static FObjectCore Instance;
			return Instance;
		}

		void FObjectCore::Dispose()
		{
			BS_PROFILE_FUNCTION();

			for (TPair<const IObject*, FClass*> ObjectClassPair : ObjectClassMappings)
			{
				if (ObjectClassPair.second)
				{
					ObjectClassPair.first->~IObject();
					delete ObjectClassPair.second;
				}
			}

			for (TPair<const uint32_t, IObjectAllocator*> ObjectAllocatorPair : ObjectAllocators)
			{
				delete ObjectAllocatorPair.second;
			}

			ObjectAllocators.clear();
			UniqueIDObjectMappings.clear();
			ObjectClassMappings.clear();
			ObjectFreeSlots.clear();
		}

		void FObjectCore::Destruct(IObject* const Object)
		{
			BS_PROFILE_FUNCTION();

			FObjectCore& Instance = FObjectCore::GetInstance();

			const size_t ObjectUniqueID = Object->UniqueID;

			Instance.ObjectFreeSlots.push_front(ObjectUniqueID);
			Instance.UniqueIDObjectMappings[ObjectUniqueID] = nullptr;

			FClass*& ObjectClass = Instance.ObjectClassMappings[Object];

			const uint32_t ObjectTypeID = Object->TypeID;
			const size_t ObjectSize = Object->GetClass()->GetSize();

			Object->~IObject();

			IObjectAllocator* const ObjectAllocator = FindObjectAllocator(ObjectTypeID);

			ObjectAllocator->Deallocate(Object, ObjectSize);
			delete std::exchange(ObjectClass, nullptr);
		}

		IObject* FObjectCore::FindObjectByUniqueID(const size_t UniqueID)
		{
			FObjectCore& Instance = GetInstance();
			TUnorderedMap<size_t, IObject*>& Objects = Instance.UniqueIDObjectMappings;
			TUnorderedMap<size_t, IObject*>::iterator ObjectIt = Objects.find(UniqueID);

			if (ObjectIt != Objects.end())
			{
				return ObjectIt->second;
			}

			return nullptr;
		}

		FObjectCore::IObjectAllocator* FObjectCore::FindObjectAllocator(const uint32_t ObjectTypeID)
		{
			FObjectAllocatorMap& ObjectAllocators = GetInstance().ObjectAllocators;
			FObjectAllocatorMap::iterator AllocatorIt = ObjectAllocators.find(ObjectTypeID);
			BS_ASSERT(AllocatorIt != ObjectAllocators.end() && AllocatorIt->second, "Trying to get not existing ObjectAllocator");
			return AllocatorIt->second;
		}
	}

	void DeleteObject(IObject* const Object)
	{
		Private::FObjectCore::Destruct(Object);
	}
}
