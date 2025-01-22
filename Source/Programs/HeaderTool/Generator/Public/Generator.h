#pragma once

#include "TypeInfo.h"

#include <cassert>
#include <format>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Bloodshot::HeaderTool::Private
{
	class FGenerator final
	{
	public:
		std::stringstream Generate(const std::unordered_map<std::string, FClassInfo>& ClassInfos);

	private:
		std::stringstream OutputStream;
		std::unordered_map<std::string, FClassInfo> ClassInfos;
		size_t PushedScopes{};

		void WriteLine(std::string_view String);

		template<typename... ArgTypes>
		void WriteLine(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			for (size_t i = 0; i < PushedScopes; ++i)
			{
				OutputStream << "\t";
			}

			OutputStream << std::format(Format, std::forward<ArgTypes>(Args)...) << "\n";
		}

		void EmptyLine();

		void PushScope();
		void PopScope();

		void GenerateReflectionAPI();
		void GenerateSerializationAPI();
		void GenerateReplicationAPI();
	};
}
