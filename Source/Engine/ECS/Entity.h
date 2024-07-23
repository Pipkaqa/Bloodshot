#pragma once

#include "ECS.h"
#include "IEntity.h"

#include <utility>

namespace Bloodshot
{
	template<typename T>
	class Entity abstract : public IEntity
	{
		ECS_PART;

	public:
		Entity() {}
		~Entity() {}

		void operator delete(void* ptr) = delete;
		void operator delete[](void* ptr) = delete;

		static const EntityTypeID s_TypeID;

		FORCEINLINE NODISCARD EntityTypeID GetTypeID() const noexcept override
		{
			return s_TypeID;
		}

		FORCEINLINE NODISCARD const char* GetTypeName() const noexcept override
		{
			static const char* typeName{typeid(T).name()};
			return typeName;
		}

		//Test first, than define where is good place for this
		//template<typename T, typename... Args>
		//T* AddComponent(Args&&... args)
		//{
		//	return ECS::AddComponent<T>(this, std::forward<Args>(args)...);
		//}
		//
		//template<typename T>
		//void RemoveComponent()
		//{
		//	ECS::RemoveComponent<T>(this);
		//}
		//
		//template<typename T>
		//T* GetComponent() const
		//{
		//	return ECS::GetComponent<T>(this);
		//}
		//
		//template<typename T>
		//bool HasComponent() const
		//{
		//	return ECS::HasComponent<T>(this);
		//}

	private:
		using IEntity::m_UniqueID;

		friend class EntityManager;
	};

	template<typename T>
	const EntityTypeID Entity<T>::s_TypeID = TypeInfo<IEntity>::GetTypeID<T>();

	static const EntityID InvalidEntityID = TypeInfo<IEntity>::s_InvalidInstanceID;
	static const EntityTypeID InvalidEntityTypeID = TypeInfo<IEntity>::s_InvalidTypeID;
}
