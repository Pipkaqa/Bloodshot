#include "PerspectiveCamera.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	IPerspectiveCamera::IPerspectiveCamera(const float Fov, const float AspectRatio, const float Near, const float Far)
		: ProjectionMatrix(glm::perspective(Fov, AspectRatio, Near, Far))
	{
	}

	void IPerspectiveCamera::SetProjectionMatrix(const float Fov, const float AspectRatio, const float Near, const float Far)
	{
		BS_PROFILE_FUNCTION();

		ProjectionMatrix = glm::perspective(Fov, AspectRatio, Near, Far);
	}
}
