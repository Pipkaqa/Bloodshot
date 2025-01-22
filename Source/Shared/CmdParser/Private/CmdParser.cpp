#include "CmdParser.h"

namespace Bloodshot::Shared
{
	FCmdParser::FCmdParser(int ArgCount, char** Args)
		: ArgCount(ArgCount)
	{
		for (size_t i = 0; i < ArgCount; ++i)
		{
			this->Args.push_back(Args[0]);
		}
	}

	FCmdParser::FCmdParser(int ArgCount, char** Args, const std::vector<std::string>& Options)
		: FCmdParser(ArgCount, Args)
	{
		PossibleOptions = Options;
		Parse();
	}

	FCmdParser::FCmdParser(int ArgCount, const std::vector<std::string>& Args, const std::vector<std::string>& Options)
	{
		this->Args = Args;
		PossibleOptions = Options;
		Parse();
	}

	const std::vector<std::string>& FCmdParser::GetPassedArgs() const
	{
		return Args;
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
		return PossibleOptions.size() == ParsedOptions.size();
	}

	void FCmdParser::AddOption(std::string_view OptionName)
	{
		PossibleOptions.push_back(OptionName.data());
	}

	FCmdOption FCmdParser::GetOption(std::string_view OptionName) const
	{
		for (const FCmdOption& Option : ParsedOptions)
		{
			if (Option.Name == OptionName)
			{
				return Option;
			}
		}

		throw std::runtime_error("Trying to get not added option");
	}

	void FCmdParser::Parse()
	{
		for (std::string_view Arg : Args)
		{
			std::optional<FCmdOption> Result = TryParseOption(Arg);

			if (Result.has_value())
			{
				ParsedOptions.emplace_back(std::move(Result.value()));
			}
		}
	}

	std::optional<FCmdOption> FCmdParser::TryParseOption(std::string_view String)
	{
		for (std::string_view PossibleOption : PossibleOptions)
		{
			std::string Option = std::format("{}:", PossibleOption.data());
			const size_t Index = String.find_first_of(Option);

			if (Index != std::string::npos && Index == 0)
			{
				std::string Value = String.substr(Option.size()).data();
				return std::make_optional<FCmdOption>(PossibleOption.data(), std::move(Value));
			}
		}

		return std::optional<FCmdOption>();
	}
}
