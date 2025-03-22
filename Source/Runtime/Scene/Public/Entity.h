#pragma once

#include "Core.h"

#include "Component.h"

namespace Bloodshot
{
	class FTransformComponent;

	BSCLASS();
	class FEntity final : public IObject
	{
		friend class FEntityManager;

		GENERATED_BODY();

	public:
		virtual ~FEntity() override {}

		BSPROPERTY(Serialized);
		FString Name = "Entity";

		BSPROPERTY(Serialized, Replicated);
		bool bActive = true;

		NODISCARD FORCEINLINE FTransformComponent* GetTransformComponent()
		{
			return reinterpret_cast<FTransformComponent*>(Components[0]);
		}

		template<IsComponent T, typename... ArgTypes>
		T* AddComponent(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();
			if (T* ExistingComponent; Components.FindByClass<T>(&ExistingComponent))
			{
				BS_LOG(Trace, "Trying to add existing component to entity");
				return ExistingComponent;
			}
			IComponent* const Component = NewObject<T>(std::forward<ArgTypes>(Args)...);
			BS_LOG(Trace, "Creating component of type: {}...", Component->GetClass()->GetName());
			Component->Owner = this;
			if (FEngineState::IsSimulating())
			{
				Component->BeginPlay();
			}
			Components.EmplaceBack(Component);
			return reinterpret_cast<T*>(Component);
		}

		template<IsComponent T>
		void RemoveComponent()
		{
			BS_PROFILE_FUNCTION();
			T* ExistingComponent;
			size_t Index;
			if (!Components.FindByClass<T>(&ExistingComponent, &Index))
			{
				BS_LOG(Trace, "Trying to remove not existing component from entity");
				return;
			}
			DestroyComponent(ExistingComponent);
			Components.RemoveAtSwap(Index);
		}

		void RemoveAllComponents();

		template<IsComponent T>
		NODISCARD T* GetComponent() const
		{
			BS_PROFILE_FUNCTION();
			T* ExistingComponent = nullptr;
			if (!Components.FindByClass<T>(&ExistingComponent))
			{
				BS_LOG(Trace, "Trying to get not existing component from entity");
			}
			return ExistingComponent;
		}

		template<IsComponent T>
		NODISCARD bool HasComponent() const
		{
			BS_PROFILE_FUNCTION();
			return Components.FindByClass<T>();
		}

		void Destroy();

	private:
		TArray<IComponent*> Components;

		FORCEINLINE void DestroyComponent(IComponent* const Component) const
		{
			BS_LOG(Trace, "Destroying component of type: {}...", Component->GetClass()->GetName());
			if (FEngineState::IsSimulating())
			{
				Component->EndPlay();
			}
			DeleteObject(Component->GetObject());
		}

		FORCEINLINE void PrivateRemoveAllComponents()
		{
			for (IComponent* const Component : Components)
			{
				DestroyComponent(Component);
			}
		}
	};
}
