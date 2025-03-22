#pragma once

#include "Allocators/PoolAllocator.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
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
			using FObjectAllocatorView = FAllocatorView::NonContiguous::Generic;

			template<IsObject T>
			using TObjectAllocator = TPoolAllocator<512>::ForElementType<T>;

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

					FObjectCore& Instance = GetInstance();

					void* const ObjectMemory = GetOrCreateObjectAllocator<T>()->Allocate();
					IObject* const Object = new(ObjectMemory) T(std::forward<ArgTypes>(Args)...);
					T* const RealObject = reinterpret_cast<T*>(Object);
					FClass* const Class = ConstructClass(RealObject);

					static uint32_t Temp = 0;

					Object->UniqueID = Temp++;
					Object->TypeID = Class->GetTypeID();
					Object->ObjectClass = Class;

					Instance.Objects.Emplace(Object);
					return RealObject;
				}

				static void DeleteObject(IObject* const Object);

				NODISCARD FORCEINLINE static bool Contains(const IObject* const Object)
				{
					return GetInstance().Objects.Contains(const_cast<IObject*>(Object));
				}

				template<IsObject T>
				NODISCARD static FClass* ConstructClass(T* const Object, void* const Memory = nullptr);

				template<typename T>
				static FClass* TryConstructOrDefaultClass(T* const Object, const char* ClassName, void* const Memory = nullptr)
				{
					if constexpr (IsObject<T>)
					{
						return ConstructClass(Object, Memory);
					}
					else
					{
						return Memory
							? new(Memory) FClass(ClassName, "", nullptr, 0, nullptr, 0, nullptr, 0, false, false, false, sizeof(T), IReservedValues::NoneTypeID)
							: new FClass(ClassName, "", nullptr, 0, nullptr, 0, nullptr, 0, false, false, false, sizeof(T), IReservedValues::NoneTypeID);
					}
				}

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
					Objects.Reserve(32768);
				}

				using FObjectAllocatorMap = TMap<uint32_t, FObjectAllocatorView>;
				FObjectAllocatorMap ObjectAllocators;

				TSet<IObject*> Objects;

				NODISCARD static FObjectAllocatorView FindObjectAllocator(const uint32_t ObjectTypeID);

				template<IsObject T>
				NODISCARD static TObjectAllocator<T>* GetOrCreateObjectAllocator()
				{
					using FObjectAllocator = TObjectAllocator<T>;

					BS_PROFILE_FUNCTION();
					FObjectAllocatorMap& ObjectAllocators = GetInstance().ObjectAllocators;
					const uint32_t ObjectTypeID = T::StaticClass()->GetTypeID();
					FObjectAllocatorView* AllocatorView = ObjectAllocators.Find(ObjectTypeID);

					if (AllocatorView)
					{
						return (FObjectAllocator*)AllocatorView->GetAllocatorPtr();
					}

					FObjectAllocator* Allocator = new FObjectAllocator();
					FObjectAllocatorView View(*Allocator);
					ObjectAllocators.Emplace(ObjectTypeID, View);
					return Allocator;
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
