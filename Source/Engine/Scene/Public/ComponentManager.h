#pragma once

#include "AllocatorTraits.h"
#include "AssertionMacros.h"
#include "BlockAllocator.h"
#include "Component.h"
#include "EntityManager.h"
#include "Logging/LoggingMacros.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

#include <unordered_map>

namespace Bloodshot
{
	template<typename T>
	concept CIsComponent = std::is_base_of_v<IComponent, T>;

	template<CIsComponent T>
	using TComponentIterator = TBlockAllocator<T>::FIterator;

	class FComponentManager final : public TSingleton<FComponentManager>
	{
		friend class FScene;

	public:
		using IComponentAllocator = IAllocator;
		template<CIsComponent T>
		using TComponentAllocator = TBlockAllocator<T>;
		using FTypeIDComponentAllocatorUnorderedMap = std::unordered_map<TypeID_t, TReference<IComponentAllocator>>;

		FComponentManager();

		static inline size_t ComponentStorageRowsGrow = 1024;
		static inline size_t ComponentStorageColsGrow = 32;

		FTypeIDComponentAllocatorUnorderedMap ComponentAllocatorsMap;
		std::vector<std::vector<InstanceID_t>> EntityComponentsTable;
		std::vector<TReference<IComponent>> ComponentsVec;
		std::list<InstanceID_t> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

		template<CIsComponent T, typename... ArgTypes>
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

				const InstanceID_t ComponentInstanceID = Instance->EntityComponentsTable[EntityInstanceID][ComponentTypeID];

				return ReinterpretCast<T*>(Instance->ComponentsVec[ComponentInstanceID]);
			}

			TReference<FComponentAllocator> Allocator = GetOrCreateComponentAllocator<T>();

			void* const Memory = IAllocatorTraits<FComponentAllocator>::Allocate(*Allocator, 1);

			IComponent* const Component = new(Memory) T(std::forward<ArgTypes>(Args)...);

			Component->InstanceID = Store(Component, EntityInstanceID, ComponentTypeID);
			Component->TypeID = ComponentTypeID;
			Component->Info = {sizeof(T), TTypeInfo<T>::GetTypeName()};
			Component->Owner = Entity;

			Component->BeginPlay();

			return ReinterpretCast<TReference<T>>(Component);
		}

		template<CIsComponent T>
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

			const InstanceID_t ComponentInstanceID = Instance->EntityComponentsTable[EntityInstanceID][ComponentTypeID];

			TReference<IComponent> Component = Instance->ComponentsVec[ComponentInstanceID];

			Component->EndPlay();

			Component->~IComponent();

			GetComponentAllocator(ComponentTypeID)->Deallocate(Component, Component->Info.Size);

			Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}

		static void RemoveAllComponents(TReference<FEntity> Entity);

		template<CIsComponent T>
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

			const InstanceID_t ComponentInstanceID = Instance->EntityComponentsTable[EntityInstanceID][ComponentTypeID];

			return ReinterpretCast<TReference<T>>(Instance->ComponentsVec[ComponentInstanceID]);
		}

		template<CIsComponent T>
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

		template<CIsComponent T>
		NODISCARD static inline TComponentIterator<T> Begin()
		{
			return ReinterpretCast<TBlockAllocator<T>*>(GetOrCreateComponentAllocator<T>())->Begin();
		}

		template<CIsComponent T>
		NODISCARD static inline TComponentIterator<T> End()
		{
			return ReinterpretCast<TBlockAllocator<T>*>(GetOrCreateComponentAllocator<T>())->End();
		}

	private:
		template<CIsComponent T>
		NODISCARD static TReference<TComponentAllocator<T>> GetOrCreateComponentAllocator()
		{
			using FComponentAllocator = TComponentAllocator<T>;

			BS_PROFILE_FUNCTION();

			FTypeIDComponentAllocatorUnorderedMap& ComponentAllocatorsMap = Instance->ComponentAllocatorsMap;

			const TypeID_t ComponentTypeID = TTypeInfo<IComponent>::GetTypeID<T>();

			FTypeIDComponentAllocatorUnorderedMap::const_iterator It = ComponentAllocatorsMap.find(ComponentTypeID);

			if (It != ComponentAllocatorsMap.end() && It->second)
			{
				return ReinterpretCast<TComponentAllocator<T>*>(It->second);
			}

			// BSTODO: Temp
			FComponentAllocator* const Allocator = new FComponentAllocator(1024, 64);

			BS_LOG(Trace, "Creating ComponentAllocator of type: {0}...", TTypeInfo<T>::GetTypeName());

			ComponentAllocatorsMap[ComponentTypeID] = Allocator;

			return Allocator;
		}

		NODISCARD static TReference<IComponentAllocator> GetComponentAllocator(const TypeID_t ComponentTypeID);

		NODISCARD static bool Contains(const InstanceID_t EntityInstanceID, const TypeID_t ComponentTypeID);

		NODISCARD static InstanceID_t Store(TReference<IComponent> Component,
			const InstanceID_t EntityInstanceID,
			const TypeID_t ComponentTypeID);

		static void Unstore(const InstanceID_t EntityID, const InstanceID_t ComponentID, const TypeID_t ComponentTypeID);

		static void Resize(const size_t NewSize);
	};
}
