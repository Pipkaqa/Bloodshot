#pragma once

#include "Core.h"

#include "Component.h"
#include "EntityManager.h"

namespace Bloodshot
{
	template<typename T>
	concept IsComponent = std::is_base_of_v<IComponent, T>;

	template<IsComponent T>
	using TComponentIterator = TBlockAllocator<T>::FIterator;

	class FComponentManager final : public TManager<FComponentManager>
	{
		friend class IEngineContext;
		friend class FScene;

	public:
		using IComponentAllocator = IAllocator;

		template<IsComponent T>
		using TComponentAllocator = TBlockAllocator<T>;

		using FEntityComponentTable = TArray<TArray<FInstanceID>>;
		using FComponentArray = TArray<TReference<IComponent>>;

		static inline size_t EntityComponentTableExpansion = 1024;

		template<IsComponent T, typename... ArgTypes>
		static TReference<T> AddComponent(TReference<FEntity> Entity, ArgTypes&&... Args)
		{
			using FComponentAllocator = TComponentAllocator<T>;

			BS_PROFILE_FUNCTION();

			const FInstanceID EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Trying to add Component to not existing Entity");
				return nullptr;
			}

			const FTypeID ComponentTypeID = FTypeID::Get<IComponent, T>();

			if (Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Trying to add already existing Component");

				const FInstanceID ComponentInstanceID = Instance->EntityComponentTable[EntityInstanceID][ComponentTypeID];

				return Instance->Components[ComponentInstanceID].As<T>();
			}

			TReference<IComponent> Component = NewObject<T>(std::forward<ArgTypes>(Args)...);

			Component->InstanceID = Store(Component, EntityInstanceID, ComponentTypeID);
			Component->TypeID = ComponentTypeID;
			Component->Owner = Entity;

			Component->BeginPlay();

			return Component.As<T>();
		}

		template<IsComponent T>
		static void RemoveComponent(TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const FInstanceID EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Trying to remove Component from not existing Entity");
				return;
			}

			const FTypeID ComponentTypeID = FTypeID::Get<IComponent, T>();

			if (!Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Trying to remove not existing Component");
				return;
			}

			const FInstanceID ComponentInstanceID = Instance->EntityComponentTable[EntityInstanceID][ComponentTypeID];

			TReference<IComponent> Component = Instance->Components[ComponentInstanceID];

			Component->EndPlay();
			DeleteObject(Component->GetObject());
			Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}

		static void RemoveAllComponents(TReference<FEntity> Entity);

		template<IsComponent T>
		NODISCARD static TReference<T> GetComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const FInstanceID EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Trying to get Component from not existing Entity");
				return nullptr;
			}

			const FTypeID ComponentTypeID = FTypeID::Get<IComponent, T>();

			if (!Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Trying to get not existing Component");
				return nullptr;
			}

			const FInstanceID ComponentInstanceID = Instance->EntityComponentTable[EntityInstanceID][ComponentTypeID];

			return Instance->Components[ComponentInstanceID].As<T>();
		}

		template<IsComponent T>
		NODISCARD static bool HasComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const FInstanceID EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Trying to check Component at not existing Entity");
				return false;
			}

			const FTypeID ComponentTypeID = FTypeID::Get<IComponent, T>();

			if (!Contains(EntityInstanceID, ComponentTypeID)) return false;

			return true;
		}

		//template<IsComponent T>
		//NODISCARD static inline TComponentIterator<T> Begin()
		//{
		//	return GetOrCreateComponentAllocator<T>()->Begin();
		//}
		//
		//template<IsComponent T>
		//NODISCARD static inline TComponentIterator<T> End()
		//{
		//	return GetOrCreateComponentAllocator<T>()->End();
		//}

	private:
		FComponentManager();

		FEntityComponentTable EntityComponentTable;
		FComponentArray Components;
		TList<FInstanceID> FreeSlots;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD static bool Contains(const FInstanceID EntityInstanceID, const FTypeID ComponentTypeID);

		NODISCARD static FInstanceID Store(TReference<IComponent> Component,
			const FInstanceID EntityInstanceID,
			const FTypeID ComponentTypeID);

		static void Unstore(const FInstanceID EntityID, const FInstanceID ComponentID, const FTypeID ComponentTypeID);

		static void ExpandEntityComponentTable();
		static void ExpandComponentArray();
	};
}
