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

	class FComponentManager final : public TSingleton<FComponentManager>
	{
		friend class FScene;

	public:
		using IComponentAllocator = IAllocator;

		template<IsComponent T>
		using TComponentAllocator = TBlockAllocator<T>;

		using FAllocatorMap = TUnorderedMap<TypeID_t, TUniquePtr<IComponentAllocator>>;
		using FEntityComponentTable = TVector<TVector<InstanceID_t>>;
		using FComponentVector = TVector<TReference<IComponent>>;

		FComponentManager();

		static inline size_t EntityComponentTableExpansion = 1024;

		FAllocatorMap Allocators;
		FEntityComponentTable EntityComponentTable;
		FComponentVector Components;
		TList<InstanceID_t> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

		template<IsComponent T, typename... ArgTypes>
		static TReference<T> AddComponent(TReference<FEntity> Entity, ArgTypes&&... Args)
		{
			using FComponentAllocator = TComponentAllocator<T>;

			BS_PROFILE_FUNCTION();

			const InstanceID_t EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Attempting to add Component to not existing Entity");
				return nullptr;
			}

			const TypeID_t ComponentTypeID = TTypeInfo<IComponent>::GetTypeID<T>();

			if (Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Attempting to add already existing Component");

				const InstanceID_t ComponentInstanceID = Instance->EntityComponentTable.at(EntityInstanceID).at(ComponentTypeID);

				return Instance->Components.at(ComponentInstanceID).As<T>();
			}

			TReference<FComponentAllocator> Allocator = GetOrCreateComponentAllocator<T>();

			void* const Memory = IAllocatorTraits<FComponentAllocator>::Allocate(*Allocator, 1);

			TReference<IComponent> Component = new(Memory) T(std::forward<ArgTypes>(Args)...);

			Component->InstanceID = Store(Component, EntityInstanceID, ComponentTypeID);
			Component->TypeID = ComponentTypeID;
			//Component->Info = {sizeof(T), TTypeInfo<T>::GetTypeName()};
			Component->Owner = Entity;

			Component->BeginPlay();

			return Component.As<T>();
		}

		template<IsComponent T>
		static void RemoveComponent(TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const InstanceID_t EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Attempting to remove Component from not existing Entity");
				return;
			}

			const TypeID_t ComponentTypeID = TTypeInfo<IComponent>::GetTypeID<T>();

			if (!Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Attempting to remove not existing Component");
				return;
			}

			const InstanceID_t ComponentInstanceID = Instance->EntityComponentTable.at(EntityInstanceID).at(ComponentTypeID);

			TReference<IComponent> Component = Instance->Components.at(ComponentInstanceID);

			Component->EndPlay();
			Component->~IComponent();

			GetComponentAllocator(ComponentTypeID)->Deallocate(Component.GetRawPtr(), Component->Info.Size);
			Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}

		static void RemoveAllComponents(TReference<FEntity> Entity);

		template<IsComponent T>
		NODISCARD static TReference<T> GetComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const InstanceID_t EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Attempting to get Component from not existing Entity");
				return nullptr;
			}

			const TypeID_t ComponentTypeID = TTypeInfo<IComponent>::GetTypeID<T>();

			if (!Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Attempting to get not existing Component");
				return nullptr;
			}

			const InstanceID_t ComponentInstanceID = Instance->EntityComponentTable.at(EntityInstanceID).at(ComponentTypeID);

			return Instance->Components.at(ComponentInstanceID).As<T>();
		}

		template<IsComponent T>
		NODISCARD static bool HasComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			const InstanceID_t EntityInstanceID = Entity->InstanceID;

			if (!FEntityManager::Contains(EntityInstanceID))
			{
				BS_LOG(Error, "Attempting to check Component at not existing Entity");
				return false;
			}

			const TypeID_t ComponentTypeID = TTypeInfo<IComponent>::GetTypeID<T>();

			if (!Contains(EntityInstanceID, ComponentTypeID)) return false;

			return true;
		}

		template<IsComponent T>
		NODISCARD static inline TComponentIterator<T> Begin()
		{
			return GetOrCreateComponentAllocator<T>()->Begin();
		}

		template<IsComponent T>
		NODISCARD static inline TComponentIterator<T> End()
		{
			return GetOrCreateComponentAllocator<T>()->End();
		}

	private:
		template<IsComponent T>
		NODISCARD static TReference<TComponentAllocator<T>> GetOrCreateComponentAllocator()
		{
			using FComponentAllocator = TComponentAllocator<T>;

			BS_PROFILE_FUNCTION();

			FAllocatorMap& Allocators = Instance->Allocators;

			const TypeID_t ComponentTypeID = TTypeInfo<IComponent>::GetTypeID<T>();

			FAllocatorMap::iterator It = Allocators.find(ComponentTypeID);

			if (It != Allocators.end() && It->second)
			{
				return It->second.GetReference().As<FComponentAllocator>();
			}

			BS_LOG(Trace, "Creating ComponentAllocator of type: {}...", TTypeInfo<T>::GetTypeName());

			// BSTODO: Temp
			TUniquePtr<IComponentAllocator> Allocator = TUniquePtr<IComponentAllocator>(new FComponentAllocator(1024, 64));
			TReference<FComponentAllocator> AllocatorReference = Allocator.GetReference().As<FComponentAllocator>();

			Allocators.emplace(ComponentTypeID, std::move(Allocator));

			return AllocatorReference;
		}

		NODISCARD static TReference<IComponentAllocator> GetComponentAllocator(const TypeID_t ComponentTypeID);

		NODISCARD static bool Contains(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID);

		NODISCARD static InstanceID_t Store(TReference<IComponent> Component,
			const InstanceID_t EntityInstanceID,
			const TypeID_t ComponentTypeID);

		static void Unstore(const InstanceID_t EntityID, const InstanceID_t ComponentID, const TypeID_t ComponentTypeID);

		static void ExpandEntityComponentTable();
		static void ExpandComponentVector();
	};
}
