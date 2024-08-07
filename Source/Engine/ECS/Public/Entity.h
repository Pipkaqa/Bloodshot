#pragma once

#include "IEntity.h"

namespace Bloodshot
{
	template<typename T>
	class TEntity abstract : public IEntity
	{
	public:
		void operator delete(void* Block) = delete;
		void operator delete[](void* Block) = delete;

		static inline const TypeID_t TypeID = TTypeInfo<IEntity>::GetTypeID();

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
		using IEntity::UniqueID;
	};
}
