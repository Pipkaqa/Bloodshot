#pragma once

#include "IComponent.h"

namespace Bloodshot
{
	template<typename T>
	class TComponent abstract : public IComponent
	{
	public:
		void operator delete(void* Block) = delete;
		void operator delete[](void* Block) = delete;

		static inline const TypeID_t TypeID = TTypeInfo<IComponent>::GetTypeID();

		NODISCARD virtual TypeID_t GetTypeID() const noexcept override
		{
			return TypeID;
		}

		NODISCARD virtual const char* GetTypeName() const noexcept override
		{
			static const char* TypeName{typeid(T).name()};
			return TypeName;
		}

	private:
		using IComponent::UniqueID;
		using IComponent::Owner;
	};
}
