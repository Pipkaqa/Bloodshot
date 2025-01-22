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
		glm::vec3 ForwardVector;

		glm::vec3 RightVector;

		glm::vec3 UpVector;
	};
}
