#pragma once

#include <cstdint>
#include <filesystem>

namespace Bloodshot
{
	enum class EFileOpenMode : uint8_t
	{
		Ate = 4,
		Append = 8,
		Truncate = 16
	};

	struct IFileIO abstract final
	{
		FORCEINLINE static void CreateIfNotExists(std::string_view Path)
		{
			if (!std::filesystem::exists(Path))
			{
				std::filesystem::create_directory(Path);
			}
		}

		NODISCARD static std::string ReadFile(std::string_view Path);
	};
}
