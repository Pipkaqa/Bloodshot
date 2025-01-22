#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Bloodshot::Shared
{
	struct FCmdOption
	{
		std::string Name;
		std::string Value;
	};

	class FCmdParser
	{
	public:
		FCmdParser(int ArgCount, char** Args);
		FCmdParser(int ArgCount, char** Args, const std::vector<std::string>& Options);
		FCmdParser(int ArgCount, const std::vector<std::string>& Args, const std::vector<std::string>& Options);

		const std::vector<std::string>& GetPassedArgs() const;

		bool HasOption(std::string_view OptionName) const;
		bool HasAllOptions() const;

		FCmdOption GetOption(std::string_view OptionName) const;

		void AddOption(std::string_view OptionName);
		void Parse();

	private:
		int ArgCount;
		std::vector<std::string> Args;
		std::vector<std::string> PossibleOptions;
		std::vector<FCmdOption> ParsedOptions;

		std::optional<FCmdOption> TryParseOption(std::string_view String);
	};
}
