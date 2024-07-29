#pragma once

#include "IComponent.h"

namespace Bloodshot
{
	template<typename T>
	class Component abstract : public IComponent
	{
		ECS_MODULE;

	public:
		Component() {}
		~Component() {}

		void operator delete(void* block) = delete;
		void operator delete[](void* block) = delete;

		static const ComponentTypeID s_TypeID;

		NODISCARD FORCEINLINE ComponentTypeID GetTypeID() const noexcept override
		{
			return s_TypeID;
		}

		NODISCARD FORCEINLINE const char* GetTypeName() const noexcept override
		{
			static const char* typeName{typeid(T).name()};
			return typeName;
		}

	private:
		using IComponent::m_UniqueID;
		using IComponent::m_Owner;
	};

	template<typename T>
	const ComponentTypeID Component<T>::s_TypeID = TypeInfo<IComponent>::GetTypeID<T>();

	static const ComponentID InvalidComponentID = TypeInfo<IComponent>::s_InvalidInstanceID;
	static const ComponentTypeID InvalidComponentTypeID = TypeInfo<IComponent>::s_InvalidTypeID;
}
