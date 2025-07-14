#include "CLIser/parser.hpp"

#include <format>
#include <print>
#include <ranges>

#include <print>


namespace CLIser {
	[[nodiscard]]
	auto Parser::create(CreateInfos &&createInfos) noexcept -> std::expected<Parser, std::string> {
		Parser parser {};
		parser.m_commandName = std::string_view{createInfos.args[0]};

		std::optional<std::string_view> currentArg {std::nullopt};
		for (const auto arg : createInfos.args.subspan(1)
			| std::views::transform([](auto str) {return std::string_view{str};})
		) {
			if (*arg.begin() != '-') {
				if (!currentArg) {
					parser.m_rawUnnamedArgs.push_back(arg);
					continue;
				}
				parser.m_rawArgs[*currentArg] = arg;
				currentArg = std::nullopt;
				continue;
			}

			const auto firstNonDash {arg.find_first_not_of('-')};
			const auto argName {arg.substr(firstNonDash)};
			if (argName.empty())
				return std::unexpected(std::format("Invalid argument : {}", arg));
			if (firstNonDash > 2)
				return std::unexpected(std::format("Invalid number of dash ({}) : {}", firstNonDash, arg));
			if (firstNonDash == 2 && argName.size() == 1)
				return std::unexpected(std::format("Invalid number of dash (2) for short argument : {}", arg));
			if (firstNonDash == 2 || argName.size() == 1) {
				currentArg = argName;
				parser.m_rawArgs[argName] = std::nullopt;
				continue;
			}
			parser.m_rawArgs[argName.substr(0, 1)] = argName.substr(1);
		}

		/*for (const auto &arg : parser.m_rawArgs)
			std::println("'{}'='{}'", arg.first, arg.second.value_or("<none>"));
		std::println("unnamed : {}", parser.m_rawUnnamedArgs);*/
		return parser;
	}


	template <>
	auto Parser::m_parseString<std::string_view> (std::string_view value) const noexcept
		-> std::optional<std::string_view>
	{
		return value;
	}


	template <>
	auto Parser::m_parseString<std::string> (std::string_view value) const noexcept -> std::optional<std::string> {
		return std::string{value};
	}


	template <>
	auto Parser::m_parseString<bool> (std::string_view value) const noexcept -> std::optional<bool> {
		using namespace std::string_view_literals;
		constexpr std::array trueValues {"true"sv, "1"sv, "on"sv};
		constexpr std::array falseValues {"false"sv, "0"sv, "off"sv};
		const auto valueLowerCase {value
			| std::views::transform([](auto c) {return std::tolower(c);})
			| std::ranges::to<std::string> ()
		};

		if (std::ranges::find(trueValues, valueLowerCase) != trueValues.end())
			return true;
		if (std::ranges::find(falseValues, valueLowerCase) != falseValues.end())
			return false;
		return std::nullopt;
	}


	auto Parser::s_printArgumentHelp(
		std::string_view option,
		std::optional<std::string_view> description,
		const CLIser::annotations::Help& help
	) noexcept -> void {
		std::string text {};
		text.reserve(help.descriptionAlignment + help.maxDescriptionWidth);
		text.append_range(std::views::repeat(' ', help.tabulationSize));
		text += option;

		if (!description) {
			std::println("{}", text);
			return;
		}

		std::string delimiter {"\n"};
		delimiter.append_range(std::views::repeat(' ', help.descriptionAlignment));
		auto lineBreakDescription {*description
			| CLIser::utils::views::chunk(help.maxDescriptionWidth)
			| std::views::join_with(delimiter)
		};

		if (text.size() > help.descriptionAlignment) {
			text += "\n";
			text.append_range(std::views::repeat(' ', help.descriptionAlignment));
		}
		else
			text.append_range(std::views::repeat(' ', help.descriptionAlignment - text.size()));

		text.append_range(lineBreakDescription);
		std::println("{}", text);
	}
}
