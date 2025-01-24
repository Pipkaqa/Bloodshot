#include "CmdParser.h"

namespace Bloodshot::Shared
{
	FCmdParser::FCmdParser(int ArgCount, char** Args)
	{
		for (size_t i = 0; i < ArgCount; ++i)
		{
			this->Args.push_back(Args[i]);
		}
	}

	FCmdParser::FCmdParser(int ArgCount, char** Args, const std::vector<std::string>& Options)
		: FCmdParser(ArgCount, Args)
	{
		PossibleOptions = Options;
	}

	const std::vector<std::string>& FCmdParser::GetPassedArgs() const
	{
		return Args;
	}

	const FCmdOption& FCmdParser::GetOption(const size_t Index) const
	{
		if (Index >= ParsedOptions.size())
		{
			// BSTODO: replace exception
			throw std::runtime_error(std::format("Trying to get not added option by index: {}", Index));
		}

		return ParsedOptions.at(Index);
	}

	const FCmdOption& FCmdParser::GetOption(std::string_view OptionName) const
	{
		for (const FCmdOption& Option : ParsedOptions)
		{
			if (Option.Name == OptionName)
			{
				return Option;
			}
		}

		// BSTODO: replace exception
		throw std::runtime_error(std::format("Trying to get not added option: {}", OptionName));
	}

	const std::string& FCmdParser::GetOptionValue(const size_t Index) const
	{
		return GetOption(Index).Value;
	}

	const std::string& FCmdParser::GetOptionValue(std::string_view OptionName) const
	{
		return GetOption(OptionName).Value;
	}

	const std::vector<FCmdOption>& FCmdParser::GetOptions() const
	{
		return ParsedOptions;
	}

	const std::string& FCmdParser::GetErrorMessage() const
	{
		return ErrorMessage;
	}

	bool FCmdParser::HasOption(std::string_view OptionName) const
	{
		if (std::find(PossibleOptions.begin(), PossibleOptions.end(), OptionName) != PossibleOptions.end())
		{
			for (const FCmdOption& Option : ParsedOptions)
			{
				if (Option.Name == OptionName)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool FCmdParser::HasAllOptions() const
	{
		if (ParsedOptions.size() < PossibleOptions.size())
		{
			return false;
		}

		for (size_t i = 0; i < PossibleOptions.size(); ++i)
		{
			if (std::find_if(ParsedOptions.begin(), ParsedOptions.end(),
				[this, &i](const FCmdOption& Option)
				{
					return Option.Name == PossibleOptions[i];
				}) == ParsedOptions.end())
			{
				return false;
			}
		}

		return true;
	}

	void FCmdParser::AddOption(std::string_view OptionName)
	{
		PossibleOptions.push_back(OptionName.data());
	}

	void FCmdParser::RemoveOption(std::string_view OptionName)
	{
		PossibleOptions.erase(std::find(PossibleOptions.begin(), PossibleOptions.end(), OptionName));
	}

	void FCmdParser::Parse()
	{
		for (const std::string& Arg : Args)
		{
			std::optional<FCmdOption> Result = TryParseOption(Arg);

			if (Result.has_value())
			{
				ParsedOptions.emplace_back(std::move(*Result));
			}
		}
	}

	void FCmdParser::BuildErrorMessage()
	{
		std::string PassedArgsStr = "Passed: ";

		for (const std::string& Arg : Args)
		{
			PassedArgsStr += Arg + " ";
		}

		PassedArgsStr.pop_back();
		
		std::string ParsedOptionsStr = "Parsed: ";

		for (const FCmdOption& Option : ParsedOptions)
		{
			ParsedOptionsStr += Option.Value + " ";
		}

		ParsedOptionsStr.pop_back();

		std::string PossibleOptionsStr = std::format("Pass {} options in any order: ", PossibleOptions.size());

		for (const std::string& Option : PossibleOptions)
		{
			PossibleOptionsStr += std::format("[{}:(Value)] ", Option);
		}

		PossibleOptionsStr.pop_back();
		PossibleOptionsStr += ", other args will be ignored";

		ErrorMessage = PassedArgsStr + "\n" + ParsedOptionsStr + "\n" + PossibleOptionsStr;
	}

	std::optional<FCmdOption> FCmdParser::TryParseOption(std::string_view String)
	{
		for (const std::string& PossibleOption : PossibleOptions)
		{
			std::string Option = std::format("{}:", PossibleOption);
			const size_t Index = String.find(Option);

			if (Index != std::string::npos && Index == 0)
			{
				std::string Value = String.substr(Option.size()).data();
				return std::make_optional<FCmdOption>(PossibleOption, std::move(Value));
			}
		}

		return std::optional<FCmdOption>();
	}
}
