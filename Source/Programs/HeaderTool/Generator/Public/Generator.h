#pragma once

#include "TypeInfo.h"

#include <format>
#include <filesystem>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Bloodshot::HeaderTool::Private
{
	struct FGeneratorOutput
	{
		std::string SourceOutput;
		std::string HeaderOutput;
	};

	class FGenerator final
	{
	public:
		FGeneratorOutput Generate(const std::vector<FClassInfo>& ClassInfos, const std::filesystem::path& PreprocessedHeaderPath);

	private:
		std::vector<FClassInfo> ClassInfos;
		std::filesystem::path PreprocessedHeaderPath;

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

		void EmptyLine();

		void PushScope();
		void PopScope();

		void GenerateReflectionAPI();
		void GenerateSerializationAPI();
		void GenerateReplicationAPI();
	};
}
