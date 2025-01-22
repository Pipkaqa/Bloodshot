#pragma once

#include "Core.h"

namespace Bloodshot
{
	struct FMesh;

	struct IAssetLoader abstract final
	{
		NODISCARD static FMesh LoadMeshFromFile(FStringView Path);
	};
}
