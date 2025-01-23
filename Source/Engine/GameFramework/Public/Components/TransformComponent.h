#pragma once

#include "Component.h"
#include "MathLibrary.h"

namespace Bloodshot
{
	class FTransformComponent final : public IComponent, public FTransform
	{
	public:
		NODISCARD FORCEINLINE const glm::vec3& GetForwardVector() const
		{
			return ForwardVector;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetRightVector() const
		{
			return RightVector;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetUpVector() const
		{
			return UpVector;
		}

		void RecalculateVectors();

	private:
		BSPROPERTY(Serialized, Replicated);
		glm::vec3 ForwardVector;

		BSPROPERTY(Serialized, Replicated);
		glm::vec3 RightVector;

		BSPROPERTY(Serialized, Replicated);
		glm::vec3 UpVector;
	};
}
