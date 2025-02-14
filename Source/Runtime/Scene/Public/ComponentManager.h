#pragma once

#include "Core.h"

#include "Component.h"
#include "EntityManager.h"

namespace Bloodshot
{
	template<typename T>
	concept IsComponent = std::is_base_of_v<IComponent, T>;

	template<IsComponent T>
	using TComponentIterator = TPoolAllocator<T>::FIterator;

	class FComponentManager final
	{
		friend class IEngineContext;
		friend class FScene;

	public:
		using FEntityComponentTable = TUnorderedMap<uint32_t, TUnorderedMap<uint32_t, TReference<IComponent>>>;

		static FComponentManager& GetInstance();

		template<IsComponent T, typename... ArgTypes>
		static TReference<T> AddComponent(TReference<FEntity> Entity, ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			FComponentManager& Instance = GetInstance();
			FEntityComponentTable& EntityComponentTable = Instance.EntityComponentTable;

			const size_t EntityUniqueID = Entity->GetUniqueID();

			if (!FEntityManager::Contains(EntityUniqueID))
			{
				BS_LOG(Error, "Trying to add component to not existing entity");
				return nullptr;
			}

			const uint32_t ComponentTypeID = ITypeID::Get<T>();

			if (Contains(EntityUniqueID, ComponentTypeID))
			{
				BS_LOG(Error, "Trying to add already existing component");
				TReference<IComponent> Component = EntityComponentTable.at(EntityUniqueID).at(ComponentTypeID);
				return Component.As<T>();
			}

			TReference<IComponent> Component = NewObject<T>(std::forward<ArgTypes>(Args)...);
			Component->Owner = Entity;
			EntityComponentTable[EntityUniqueID][ComponentTypeID] = Component;
			Component->BeginPlay();

			return Component.As<T>();
		}

		template<IsComponent T>
		static void RemoveComponent(TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const uint32_t EntityUniqueID = Entity->GetUniqueID();

			if (!FEntityManager::Contains(EntityUniqueID))
			{
				BS_LOG(Error, "Trying to remove component from not existing entity");
				return;
			}

			const uint32_t ComponentTypeID = ITypeID::Get<T>();

			if (!Contains(EntityUniqueID, ComponentTypeID))
			{
				BS_LOG(Error, "Trying to remove not existing component");
				return;
			}

			TReference<IComponent>& Component = GetInstance().EntityComponentTable.at(EntityUniqueID).at(ComponentTypeID);

			Component->EndPlay();
			DeleteObject(Component->GetObject());

			Component = nullptr;
		}

		static void RemoveAllComponents(TReference<FEntity> Entity);

		template<IsComponent T>
		NODISCARD static TReference<T> GetComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const uint32_t EntityUniqueID = Entity->GetUniqueID();

			if (!FEntityManager::Contains(EntityUniqueID))
			{
				BS_LOG(Error, "Trying to get component from not existing entity");
				return nullptr;
			}

			const uint32_t ComponentTypeID = ITypeID::Get<T>();

			if (!Contains(EntityUniqueID, ComponentTypeID))
			{
				BS_LOG(Error, "Trying to get not existing component");
				return nullptr;
			}

			return GetInstance().EntityComponentTable.at(EntityUniqueID).at(ComponentTypeID).As<T>();
		}

		template<IsComponent T>
		NODISCARD static bool HasComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const uint32_t EntityUniqueID = Entity->GetUniqueID();

			if (!FEntityManager::Contains(EntityUniqueID))
			{
				BS_LOG(Error, "Trying to check component at not existing entity");
				return false;
			}

			const uint32_t ComponentTypeID = ITypeID::Get<T>();

			if (Contains(EntityUniqueID, ComponentTypeID))
			{
				return false;
			}

			return true;
		}

		//template<IsComponent T>
		//NODISCARD static inline TComponentIterator<T> CreateComponentIterator()
		//{
		//	return GetOrCreateComponentAllocator<T>()->Begin();
		//}

	private:
		FComponentManager() {}

		FEntityComponentTable EntityComponentTable;

		NODISCARD static bool Contains(const uint32_t EntityUniqueID, const uint32_t ComponentTypeID);
	};
}
