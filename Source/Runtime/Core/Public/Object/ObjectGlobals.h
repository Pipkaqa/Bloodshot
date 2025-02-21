#pragma once

#include "Allocators/PoolAllocator.h"
#include "Containers/List.h"
#include "Containers/UnorderedMap.h"
#include "Misc/TypeID.h"
#include "Object/Class.h"
#include "Object/Object.h"
#include "Platform/Platform.h"
#include "Profiling/ProfilingMacros.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	namespace Private
	{
		class IEngineContext;

		template<IsObject T>
		using TObjectIterator = TPoolAllocator<T>;

		class FObjectCore final
		{
			friend class IEngineContext;

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
				FClass* const Class = ConstructClass((T*)Object);

				Object->TypeID = ITypeID::Get<T>();
				Object->ObjectClass = Class;
				Instance.ObjectClassMappings.emplace(Object, Class);

				if (!Instance.ObjectFreeSlots.size())
				{
					const size_t OldSize = Instance.UniqueIDObjectMappings.size();
					const size_t NewSize = OldSize + 1024ull;

					for (size_t i = OldSize; i < NewSize; ++i)
					{
						Instance.ObjectFreeSlots.push_back(i);
					}

					Instance.UniqueIDObjectMappings.reserve(NewSize);
				}

				const size_t ObjectSlot = Instance.ObjectFreeSlots.front();
				Instance.ObjectFreeSlots.pop_front();

				Object->UniqueID = (uint32_t)ObjectSlot;
				Instance.UniqueIDObjectMappings.insert_or_assign(ObjectSlot, Object);

				return (T*)Object;
			}

			static void DeleteObject(IObject* const Object);

			NODISCARD static IObject* FindObjectByUniqueID(const size_t UniqueID);

			template<typename T>
			NODISCARD static FClass* TryConstructOrDefaultClass(T* const Object, const char* ClassName)
			{
				if constexpr (std::is_base_of_v<IObject, T>)
				{
					return ConstructClass(Object);
				}
				else
				{
					return new FClass(ClassName, {}, {}, {}, {}, {}, {}, {}, {});
				}
			}

			template<IsObject T>
			NODISCARD static FClass* ConstructClass(T* Object);

			template<IsObject T>
			NODISCARD static FClass* GetStaticClass();

			template<IsObject T>
			NODISCARD static TObjectIterator<T> CreateObjectIterator()
			{
				GetOrCreateObjectAllocator<T>()->CreateIterator();
			}

		private:
			using IObjectAllocator = IAllocator;
			template<IsObject T> using TObjectAllocator = TPoolAllocator<T>;
			using FObjectAllocatorMap = TUnorderedMap<uint32_t, IObjectAllocator*>;

			FObjectCore() {}

			FObjectAllocatorMap ObjectAllocators;
			TUnorderedMap<size_t, IObject*> UniqueIDObjectMappings;
			TUnorderedMap<IObject*, FClass*> ObjectClassMappings;
			TList<size_t> ObjectFreeSlots;

			NODISCARD static IObjectAllocator* FindObjectAllocator(const uint32_t ObjectTypeID);

			template<IsObject T>
			NODISCARD static TObjectAllocator<T>* GetOrCreateObjectAllocator()
			{
				using FObjectAllocator = TObjectAllocator<T>;

				BS_PROFILE_FUNCTION();
				FObjectAllocatorMap& ObjectAllocators = GetInstance().ObjectAllocators;
				const uint32_t ObjectTypeID = ITypeID::Get<T>();
				FObjectAllocatorMap::iterator AllocatorIt = ObjectAllocators.find(ObjectTypeID);

				if (AllocatorIt != ObjectAllocators.end() && AllocatorIt->second)
				{
					return (FObjectAllocator*)AllocatorIt->second;
				}

				IObjectAllocator* Allocator = new FObjectAllocator(1024, 64);
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

	template<IsObject T, typename... ArgTypes>
	T* NewObject(ArgTypes&&... Args)
	{
		return Private::FObjectCore::NewObject<T>(std::forward<ArgTypes>(Args)...);
	}

	void DeleteObject(IObject* const Object);
}
