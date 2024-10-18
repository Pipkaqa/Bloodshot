#pragma once

#include "MathLibrary.h"
#include "Platform.h"

namespace Bloodshot
{
	// BSTODO: Implement Orthographic side, rename to ICamera
	class IPerspectiveCamera abstract
	{
	public:
		IPerspectiveCamera(const float Fov, const float AspectRatio, const float Near, const float Far);
		virtual ~IPerspectiveCamera() {}

		NODISCARD FORCEINLINE const glm::mat4& GetProjectionMatrix() const
		{
			return ProjectionMatrix;
		}

		void SetProjectionMatrix(const float Fov, const float AspectRatio, const float Near, const float Far);

	protected:
		glm::mat4 ProjectionMatrix;
	};
}
