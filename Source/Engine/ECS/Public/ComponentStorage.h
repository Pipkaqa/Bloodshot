#pragma once

#include "Component.h"
#include "IEntity.h"
#include "Platform.h"

#include <unordered_map>

namespace Bloodshot
{
	class IScene;

	struct FComponentStorage final
	{
		FComponentStorage(IScene* const Context);
		~FComponentStorage();

		using FComponentInstanceIDMap = std::unordered_map<TypeID_t, InstanceID_t>;
		using FEntityComponentTable = std::unordered_map<InstanceID_t, FComponentInstanceIDMap>;
		using FComponentVec = std::vector<IComponent*>;

		IScene* const Context;

		FEntityComponentTable EntityComponentsTable;
		FComponentVec ComponentsVec;

		void Store(IEntity* const Entity, const InstanceID_t EntityInstanceID, IComponent* const Component, const TypeID_t ComponentTypeID);
		void Unstore(const InstanceID_t EntityInstanceID, const InstanceID_t ComponentInstanceID, const TypeID_t ComponentTypeID);
		void Clear();

		NODISCARD InstanceID_t GetComponentInstanceID(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID) const;

		NODISCARD IComponent* GetComponent(const InstanceID_t ComponentInstanceID);
		NODISCARD const IComponent* GetComponent(const InstanceID_t ComponentInstanceID) const;

		NODISCARD IComponent* GetComponent(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID);
		NODISCARD const IComponent* GetComponent(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID) const;

		NODISCARD FComponentVec& GetComponentVec();
		NODISCARD const FComponentVec& GetComponentVec() const;

		NODISCARD FComponentInstanceIDMap& GetMappedEntityComponents(const InstanceID_t EntityInstanceID);
		NODISCARD const FComponentInstanceIDMap& GetMappedEntityComponents(const InstanceID_t EntityInstanceID) const;

		NODISCARD bool Contains(const InstanceID_t ComponentInstanceID) const;
		NODISCARD bool Contains(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID) const;
	};
}
