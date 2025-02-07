#pragma once

#include "Core.h"

namespace Bloodshot
{
	struct FMesh;

	struct IAssetLoader final
	{
		NODISCARD static FMesh LoadMeshFromFile(FStringView Path);
	};
}
