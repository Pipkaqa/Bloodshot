#pragma once

#include "Core.h"

#include "Component.h"

namespace Bloodshot
{
	BSCLASS();
	class FTransformComponent final : public IComponent
	{
		GENERATED_BODY();

	public:
		FORCEINLINE FTransformComponent()
		{
			const_cast<FEventView&>(OnTransformChanged) = OnTransformChangedEvent.GetView();
		}

		const FEventView OnTransformChanged;

		NODISCARD FORCEINLINE const glm::vec3& GetPosition() const noexcept
		{
			return Transform.Position;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetRotation() const noexcept
		{
			return Transform.Rotation;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetScale() const noexcept
		{
			return Transform.Scale;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetForwardVector() const noexcept
		{
			if (bDirty)
			{
				UpdateVectors();
			}

			return ForwardVector;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetRightVector() const noexcept
		{
			if (bDirty)
			{
				UpdateVectors();
			}

			return RightVector;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetUpVector() const noexcept
		{
			if (bDirty)
			{
				UpdateVectors();
			}

			return UpVector;
		}

		NODISCARD FORCEINLINE void SetPosition(const glm::vec3& NewPosition) noexcept
		{
			Transform.Position = NewPosition;
			OnTransformChangedEvent.Trigger();
		}

		NODISCARD FORCEINLINE void SetRotation(const glm::vec3& NewRotation) noexcept
		{
			Transform.Rotation = NewRotation;
			OnTransformChangedEvent.Trigger();
			bDirty = true;
		}

		NODISCARD FORCEINLINE void SetScale(const glm::vec3& NewScale) noexcept
		{
			Transform.Scale = NewScale;
			OnTransformChangedEvent.Trigger();
		}

		void UpdateVectors() const;

	private:
		BSPROPERTY(Serialized, Replicated);
		mutable glm::vec3 ForwardVector;

		BSPROPERTY(Serialized, Replicated);
		mutable glm::vec3 RightVector;

		BSPROPERTY(Serialized, Replicated);
		mutable glm::vec3 UpVector;

		BSPROPERTY(Serialized, Replicated);
		FTransform Transform;

		mutable FEvent OnTransformChangedEvent;

		mutable bool bDirty = true;
	};
}
