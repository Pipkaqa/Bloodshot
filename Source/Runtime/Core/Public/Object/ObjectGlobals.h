#pragma once

#include "Containers/List.h"
#include "Containers/UnorderedMap.h"
#include "Object/Class.h"
#include "Object/Object.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	class IEngineContext;

	namespace Private
	{
		class FObjectCore final
		{
			friend struct IObjectCoreInterface;
			friend struct IClassConstructor;
			friend struct IObjectConstructor;
			friend class ::Bloodshot::IEngineContext;

		public:
			~FObjectCore() {}

			static FObjectCore& GetInstance();

		private:
			FObjectCore() {}

			TUnorderedMap<IObject*, FClass*> ObjectClassMappings;
			TUnorderedMap<size_t, IObject*> UniqueIDObjectMappings;
			TList<size_t> ObjectFreeSlots;

			void Dispose();
		};

		struct IObjectCoreInterface final
		{
			static IObject* FindObjectByUniqueID(const size_t UniqueID);
		};

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
				FObjectCore& ObjectCore = FObjectCore::GetInstance();

				IObject* const Object = new T(std::forward<ArgTypes>(Args)...);
				FClass* const Class = IClassConstructor::ConstructClass((T*)Object);

				Object->ObjectClass = Class;
				ObjectCore.ObjectClassMappings.emplace(Object, Class);

				if (!ObjectCore.ObjectFreeSlots.size())
				{
					const size_t OldSize = ObjectCore.UniqueIDObjectMappings.size();
					const size_t NewSize = OldSize + 1024ull;

					for (size_t i = OldSize; i < NewSize; ++i)
					{
						ObjectCore.ObjectFreeSlots.push_back(i);
					}

					ObjectCore.UniqueIDObjectMappings.reserve(NewSize);
				}

				const size_t Slot = ObjectCore.ObjectFreeSlots.front();
				ObjectCore.ObjectFreeSlots.pop_front();

				Object->UniqueID = Slot;
				ObjectCore.UniqueIDObjectMappings.insert_or_assign(Slot, Object);

				return (T*)Object;
			}

			static void Destruct(IObject* const Object);
		};

		template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes>
		struct IFunctionCaller final
		{
			static void Call(ObjectType* Object, ArgTypes&&... Args);
		};
	}

	template<IsObject T, typename... ArgTypes>
	T* NewObject(ArgTypes&&... Args)
	{
		return Private::IObjectConstructor::Construct<T>(std::forward<ArgTypes>(Args)...);
	}

	void DeleteObject(IObject* const Object);
}
