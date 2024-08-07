#pragma once

#include "IEntity.h"

#include <vector>

namespace Bloodshot
{
	class IScene;

	struct FEntityStorage final
	{
		FEntityStorage(IScene* const Context);
		~FEntityStorage();

		using FEntityVec = std::vector<IEntity*>;

		IScene* Context;

		FEntityVec EntitiesVec;

		void Store(IEntity* const Entity);
		void Unstore(const InstanceID_t EntityInstanceID);
		void Clear();

		NODISCARD IEntity* GetEntity(const InstanceID_t EntityInstanceID);
		NODISCARD const IEntity* GetEntity(const InstanceID_t EntityInstanceID) const;

		NODISCARD FEntityVec& GetEntityVec();
		NODISCARD const FEntityVec& GetEntityVec() const;

		NODISCARD bool Contains(const InstanceID_t EntityInstanceID) const;
	};
}
