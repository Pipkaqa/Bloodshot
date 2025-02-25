#pragma once

#include "Allocators/LinearAllocator.h"
#include "Allocators/PoolAllocator.h"
#include "Containers/List.h"
#include "Containers/UnorderedMap.h"
#include "Object/Class.h"
#include "Object/Object.h"
#include "Platform/Platform.h"
#include "Profiling/ProfilingMacros.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	namespace Private
	{
		namespace Launch { class IEngineContext; }
		namespace Object
		{
			using IObjectAllocator = IAllocator;

			template<IsObject T>
			using TObjectAllocator = TPoolAllocator<T, 512>;

			template<IsObject T>
			using TObjectIterator = TObjectAllocator<T>::FIterator;

			class FObjectCore final
			{
				friend class Launch::IEngineContext;

			public:
				FORCEINLINE ~FObjectCore()
				{
					Dispose();
				}

				NODISCARD static FObjectCore& GetInstance();

				template<IsObject T, typename... ArgTypes>
				NODISCARD static T* NewObject(ArgTypes&&... Args)
				{
					BS_PROFILE_FUNCTION();
					FObjectCore& Instance = FObjectCore::GetInstance();

					void* const Memory = GetOrCreateObjectAllocator<T>()->Allocate(1);
					IObject* const Object = new(Memory) T(std::forward<ArgTypes>(Args)...);
					T* const RealObject = ReinterpretCast<T*>(Object);
					FClass* const Class = ConstructClass(RealObject);

					Object->TypeID = Class->GetTypeID();
					Object->ObjectClass = Class;
					Instance.ObjectClassMappings.emplace(Object, Class);

					TList<uint32_t>& ObjectFreeSlots = Instance.ObjectFreeSlots;
					TUnorderedMap<uint32_t, IObject*>& UniqueIDObjectMappings = Instance.UniqueIDObjectMappings;

					if (!ObjectFreeSlots.size())
					{
						const uint32_t OldSize = (uint32_t)UniqueIDObjectMappings.size();
						const uint32_t NewSize = OldSize + (uint32_t)((float)OldSize * 1.5f);
						for (uint32_t i = OldSize; i < NewSize; ++i)
						{
							ObjectFreeSlots.emplace_back(i);
						}
						UniqueIDObjectMappings.reserve(NewSize);
					}

					const uint32_t ObjectSlot = ObjectFreeSlots.front();
					ObjectFreeSlots.pop_front();
					Object->UniqueID = ObjectSlot;
					UniqueIDObjectMappings.insert_or_assign(ObjectSlot, Object);

					return RealObject;
				}

				static void DeleteObject(IObject* const Object);

				NODISCARD static IObject* FindObjectByUniqueID(const uint32_t UniqueID);

				template<typename T>
				NODISCARD static FClass* TryConstructOrDefaultClass(T* const Object, const char* ClassName)
				{
					if constexpr (std::is_base_of_v<IObject, T>)
					{
						return ConstructClass(Object);
					}
					else
					{
						return new FClass(ClassName, {}, {}, {}, {}, {}, {}, {}, {}, IReservedValues::NoneTypeID);
					}
				}

				template<IsObject T>
				NODISCARD static FClass* ConstructClass(T* Object);

				template<IsObject T>
				NODISCARD static FClass* GetStaticClass();

				template<IsObject T>
				NODISCARD FORCEINLINE static TObjectIterator<T> CreateObjectIterator()
				{
					return GetOrCreateObjectAllocator<T>()->CreateIterator();
				}

			private:
				FORCEINLINE FObjectCore()
				{
					const uint32_t InitialSize = 64000u;
					for (uint32_t i = 0; i < InitialSize; ++i)
					{
						ObjectFreeSlots.emplace_back(i);
					}
					UniqueIDObjectMappings.reserve(InitialSize);
				}

				using FObjectAllocatorMap = TUnorderedMap<uint32_t, IObjectAllocator*>;
				FObjectAllocatorMap ObjectAllocators;

				TUnorderedMap<uint32_t, IObject*> UniqueIDObjectMappings;
				TUnorderedMap<IObject*, FClass*> ObjectClassMappings;
				TList<uint32_t> ObjectFreeSlots;

				NODISCARD static IObjectAllocator* FindObjectAllocator(const uint32_t ObjectTypeID);

				template<IsObject T>
				NODISCARD static TObjectAllocator<T>* GetOrCreateObjectAllocator()
				{
					using FObjectAllocator = TObjectAllocator<T>;

					BS_PROFILE_FUNCTION();
					FObjectAllocatorMap& ObjectAllocators = GetInstance().ObjectAllocators;
					const uint32_t ObjectTypeID = T::StaticClass()->GetTypeID();
					FObjectAllocatorMap::iterator AllocatorIt = ObjectAllocators.find(ObjectTypeID);

					if (AllocatorIt != ObjectAllocators.end() && AllocatorIt->second)
					{
						return (FObjectAllocator*)AllocatorIt->second;
					}

					IObjectAllocator* Allocator = new FObjectAllocator();
					ObjectAllocators.emplace(ObjectTypeID, Allocator);
					return (FObjectAllocator*)Allocator;
				}

				void Dispose();
			};

			template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes>
			class IFunctionCaller final
			{
			public:
				static void Call(ObjectType* Object, ArgTypes&&... Args);
			};
		}
	}

	template<IsObject T, typename... ArgTypes>
	T* NewObject(ArgTypes&&... Args)
	{
		return Private::Object::FObjectCore::NewObject<T>(std::forward<ArgTypes>(Args)...);
	}

	void DeleteObject(IObject* const Object);
}
