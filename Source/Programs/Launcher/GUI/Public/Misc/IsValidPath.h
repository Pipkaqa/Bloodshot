#pragma once

#include "Platform/Platform.h"

#include <filesystem>

namespace Bloodshot::Launcher
{
	NODISCARD FORCEINLINE bool IsValidPath(const std::filesystem::path& InPath)
	{
		using namespace std::filesystem;
		path::iterator Current = InPath.begin();
		path::const_iterator End = InPath.end();
		while (Current != End)
		{
			const std::string& Filename = Current->filename().string();
			if (Filename.contains("\\")
				|| Filename.contains("/")
				|| Filename.contains(":")
				|| Filename.contains("*")
				|| Filename.contains("?")
				|| Filename.contains("\"")
				|| Filename.contains("<")
				|| Filename.contains(">")
				|| Filename.contains("|"))
			{
				return false;
			}
			++Current;
		}
		return true;
	}
}
