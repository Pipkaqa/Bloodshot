#pragma once

#include "TypeInfo.h"

#include <filesystem>
#include <format>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Bloodshot::HeaderTool
{
	struct FHeaderFileInfo
	{
		std::vector<FClassInfo> ClassInfos;
		std::filesystem::path Path;
	};

	struct FGeneratorOutput
	{
		std::string SourceOutput;
		std::string HeaderOutput;
	};

	class FGenerator final
	{
	public:
		FGeneratorOutput Generate(const FHeaderFileInfo& HeaderInfo);

	private:
		std::stringstream SourceOutputStream;
		std::stringstream HeaderOutputStream;

		std::stringstream* CurrentOutputStream;
		size_t PushedScopes = 0;

		void WriteLine(std::string_view String);

		template<typename... ArgTypes>
		void WriteLine(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			for (size_t i = 0; i < PushedScopes; ++i)
			{
				*CurrentOutputStream << "\t";
			}

			*CurrentOutputStream << std::format(Format, std::forward<ArgTypes>(Args)...) << "\n";
		}

		void PushScope();
		void PopScope();

		void Clear();

		void WriteClassConstructionImpl(const FHeaderFileInfo& HeaderInfo);
		void WriteSerializeionImpl();
		void WriteReplicationImpl();
	};
}
