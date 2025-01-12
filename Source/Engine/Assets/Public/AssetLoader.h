#pragma once

#include "Platform/Platform.h"

#include <string_view>

namespace Bloodshot
{
	struct FMesh;

	struct IAssetLoader abstract final
	{
		NODISCARD static FMesh LoadMeshFromFile(std::string_view Path);
	};
}
