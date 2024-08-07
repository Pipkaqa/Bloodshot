#pragma once

#include "AssertionMacros.h"
#include "BlockAllocator.h"
#include "ComponentStorage.h"
#include "IComponent.h"
#include "IEntity.h"
#include "Logging/LoggingMacros.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	template<typename T>
		requires std::is_base_of_v<IComponent, T>
	using TComponentIterator = TBlockAllocator<T>::FIterator;

	class FComponentManager final : public TSingleton<FComponentManager>
	{
		friend struct IECS;

	public:
		using IComponentAllocator = IAllocator;
		template<typename T>
		using TComponentAllocator = TBlockAllocator<T>;
		using FTypeIDComponentAllocatorUnorderedMap = std::unordered_map<TypeID_t, IComponentAllocator*>;

		FComponentManager();

		FTypeIDComponentAllocatorUnorderedMap ComponentAllocatorsMap;

		virtual void Init() override;
		virtual void Dispose() override;

	private:
		template<typename T, typename... ArgTypes>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static T* AddComponent(TUniquePtr<FComponentStorage>& Storage, IEntity* const Entity, ArgTypes&&... Args)
		{
			const TypeID_t ComponentTypeID = T::TypeID;

			const InstanceID_t EntityInstanceID = Entity->GetUniqueID();

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(!Storage->Contains(EntityInstanceID, ComponentTypeID), 
			// "Attempting to add already existing component");
			if (Storage->Contains(EntityInstanceID, ComponentTypeID))
			{
				BS_LOG(Error, "Attempting to add already existing Component");
				return ReinterpretCast<T*>(Storage->GetComponent(EntityInstanceID, ComponentTypeID));
			}

			IComponent* const Component = new(GetOrCreateComponentAllocator<T>()->Allocate()) T(std::forward<ArgTypes>(Args)...);

			Storage->Store(Entity, EntityInstanceID, Component, ComponentTypeID);

			Component->BeginPlay();

			return ReinterpretCast<T*>(Component);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		static void RemoveComponent(TUniquePtr<FComponentStorage>& Storage, IEntity* const Entity)
		{
			const TypeID_t ComponentTypeID = T::TypeID;

			const InstanceID_t EntityInstanceID = Entity->UniqueID;

			const InstanceID_t ComponentInstanceID = Storage->GetComponentInstanceID(EntityInstanceID, ComponentTypeID);

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(ComponentInstanceID != InvalidInstanceID && Storage->Contains(ComponentInstanceID),
			//	"Attempting to remove not existing component");
			if (ComponentInstanceID == InvalidInstanceID || !Storage->Contains(ComponentInstanceID))
			{
				BS_LOG(Error, "Attempting to remove not existing Component");
				return;
			}

			IComponent* const Component = Storage->GetComponent(ComponentInstanceID);

			Component->EndPlay();

			GetComponentAllocator(ComponentTypeID)->Deallocate(Component);

			Storage->Unstore(EntityInstanceID, ComponentInstanceID, ComponentTypeID);
		}

		static void RemoveAllComponents(TUniquePtr<FComponentStorage>& storage, IEntity* const Entity);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static T* GetComponent(const TUniquePtr<FComponentStorage>& Storage, const IEntity* const Entity)
		{
			const TypeID_t ComponentTypeID = T::TypeID;

			const InstanceID_t EntityInstanceID = Entity->UniqueID;

			const InstanceID_t ComponentInstanceID = Storage->GetComponentInstanceID(EntityInstanceID, ComponentTypeID);

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(ComponentInstanceID != InvalidInstanceID, "Attempting to get not existing component");
			if (ComponentInstanceID == InvalidInstanceID)
			{
				BS_LOG(Error, "Attempting to get not existing Component");
				return nullptr;
			}

			return ReinterpretCast<T*>(Storage->GetComponent(ComponentInstanceID));
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static bool HasComponent(const TUniquePtr<FComponentStorage>& Storage, const IEntity* Entity)
		{
			const TypeID_t ComponentTypeID = T::TypeID;

			const InstanceID_t EntityInstanceID = Entity->UniqueID;

			const InstanceID_t ComponentInstanceID = Storage->GetComponentInstanceID(EntityInstanceID, ComponentTypeID);

			if (ComponentInstanceID == InvalidInstanceID) return false;

			return true;
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static TComponentAllocator<T>* GetOrCreateComponentAllocator()
		{
			using FComponentAllocator = TComponentAllocator<T>;

			FTypeIDComponentAllocatorUnorderedMap& ComponentAllocatorsMap = Instance->ComponentAllocatorsMap;

			const TypeID_t ComponentTypeID = T::TypeID;

			FTypeIDComponentAllocatorUnorderedMap::const_iterator It = ComponentAllocatorsMap.find(ComponentTypeID);

			if (It != ComponentAllocatorsMap.end() && It->second)
			{
				return ReinterpretCast<TComponentAllocator<T>*>(It->second);
			}

			FComponentAllocator* const Allocator = new FComponentAllocator({1024, 100});

			BS_LOG(Trace, "Creating ComponentAllocator of type: {0}...", TTypeInfo<T>::GetTypeName());

			ComponentAllocatorsMap[ComponentTypeID] = Allocator;

			return Allocator;
		}

		NODISCARD static IComponentAllocator* GetComponentAllocator(const TypeID_t ComponentTypeID);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static inline TComponentIterator<T> Begin()
		{
			return GetOrCreateComponentAllocator<T>()->Begin();
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static inline TComponentIterator<T> End()
		{
			return GetOrCreateComponentAllocator<T>()->End();
		}
	};
}
