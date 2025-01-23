#pragma once

#include "TypeInfo.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Bloodshot::HeaderTool::Private
{
	class FGenerator final
	{
	public:
		void Generate(const std::vector<FClassInfo>& ClassInfos,
			const std::filesystem::path& OutputPath,
			const std::filesystem::path& HeaderPath);

	private:
		std::filesystem::path HeaderPath;

		std::ofstream SourceOutputStream;
		std::ofstream HeaderOutputStream;
		std::ofstream* CurrentOutputStream = nullptr;

		std::vector<FClassInfo> ClassInfos;
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
