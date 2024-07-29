#pragma once

#include "IEntity.h"

namespace Bloodshot
{
	template<typename T>
	class Entity abstract : public IEntity
	{
		ECS_MODULE;

	public:
		Entity() {}
		~Entity() {}

		void operator delete(void* block) = delete;
		void operator delete[](void* block) = delete;

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

	private:
		using IEntity::m_UniqueID;
	};

	template<typename T>
	const EntityTypeID Entity<T>::s_TypeID = TypeInfo<IEntity>::GetTypeID<T>();

	static const EntityID InvalidEntityID = TypeInfo<IEntity>::s_InvalidInstanceID;
	static const EntityTypeID InvalidEntityTypeID = TypeInfo<IEntity>::s_InvalidTypeID;
}
