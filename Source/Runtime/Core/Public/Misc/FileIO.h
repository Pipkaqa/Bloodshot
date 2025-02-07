#pragma once

#include "Containers/String.h"
#include "Platform/Platform.h"

#include <filesystem>

namespace Bloodshot
{
	enum class EFileOpenMode : uint8_t
	{
		Ate = 4,
		Append = 8,
		Truncate = 16
	};

	struct IFileIO final
	{
		FORCEINLINE static void CreateIfNotExists(FStringView Path)
		{
			if (!std::filesystem::exists(Path))
			{
				std::filesystem::create_directory(Path);
			}
		}

		NODISCARD static FString ReadFile(FStringView Path);
	};
}
