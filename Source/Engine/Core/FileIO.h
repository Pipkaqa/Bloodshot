#pragma once

#include "EngineFramework.h"

#include <filesystem>

namespace Bloodshot
{
	class FileIO
	{
	public:
		enum class FileOpenMode : uint8_t
		{
			Ate = 4,
			Append = 8,
			Truncate = 16
		};

		FORCEINLINE static void CreateIfNotExists(std::string_view path)
		{
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
		}

		NODISCARD static std::string ReadFile(std::string_view path);
	};
}
