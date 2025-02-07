#include "Camera.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	ICamera::ICamera(const FCameraSettings& Settings)
		: Settings(Settings)
		, ProjectionMatrix(Settings.ProjectionMode == ECameraProjectionMode::Perspective
			? glm::perspective(Settings.Fov, Settings.AspectRatio, Settings.Near, Settings.Far)
			: glm::ortho(Settings.Left, Settings.Right, Settings.Bottom, Settings.Top))
	{
	}
}
