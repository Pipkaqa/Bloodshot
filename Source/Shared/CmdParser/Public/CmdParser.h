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

		const std::vector<std::string>& GetPassedArgs() const;

		const FCmdOption& GetOption(const size_t Index) const;
		const FCmdOption& GetOption(std::string_view OptionName) const;

		const std::string& GetOptionValue(const size_t Index) const;
		const std::string& GetOptionValue(std::string_view OptionName) const;

		const std::vector<FCmdOption>& GetOptions() const;

		const std::string& GetErrorMessage() const;

		bool HasOption(std::string_view OptionName) const;
		bool HasAllOptions() const;

		void AddOption(std::string_view OptionName);
		void RemoveOption(std::string_view OptionName);

		void Parse();

		void BuildErrorMessage();

	private:
		std::vector<std::string> Args;

		std::vector<std::string> PossibleOptions;
		std::vector<FCmdOption> ParsedOptions;

		std::string ErrorMessage;

		std::optional<FCmdOption> TryParseOption(std::string_view String);
	};
}
