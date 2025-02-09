#pragma once

#include "Containers/List.h"
#include "Containers/UnorderedMap.h"
#include "Object/Class.h"
#include "Object/Object.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	namespace Private
	{
		inline TUnorderedMap<IObject*, FClass*> GObjectClassMappings;
		inline TUnorderedMap<size_t, IObject*> GUniqueIDObjectMappings;
		inline TList<size_t> GObjectFreeSlots;

		struct IClassConstructor final
		{
			template<IsObject T>
			static FClass* ConstructClass(T* Object);

			template<IsObject T>
			static FClass* GetStaticClass();
		};

		struct IObjectConstructor final
		{
			template<IsObject T, typename... ArgTypes>
			NODISCARD static T* Construct(ArgTypes&&... Args)
			{
				IObject* const Object = new T(std::forward<ArgTypes>(Args)...);
				FClass* const Class = IClassConstructor::ConstructClass((T*)Object);
				Object->ObjectClass = Class;
				GObjectClassMappings.emplace(Object, Class);
				const size_t Slot = GObjectFreeSlots.front();
				GObjectFreeSlots.pop_front();
				Object->UniqueID = Slot;
				GUniqueIDObjectMappings.insert_or_assign(Slot, Object);
				return (T*)Object;
			}

			static void Destruct(IObject* const Object);
		};

		template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes>
		struct IFunctionCaller final
		{
			static void Call(ObjectType* Object, ArgTypes&&... Args);
		};

		IObject* FindObjectByUniqueID(const size_t Slot);
	}

	template<IsObject T, typename... ArgTypes>
	T* NewObject(ArgTypes&&... Args)
	{
		return Private::IObjectConstructor::Construct<T>(std::forward<ArgTypes>(Args)...);
	}

	void DeleteObject(IObject* const Object);
}
