#include "CLIser/parser.hpp"

#include <format>
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
			if (firstNonDash == 2 || argName.size() == 1) {
				currentArg = argName;
				parser.m_rawArgs[argName] = std::nullopt;
				continue;
			}
			parser.m_rawArgs[argName.substr(0, 1)] = argName.substr(1);
		}

		for (const auto &arg : parser.m_rawArgs)
			std::println("'{}'='{}'", arg.first, arg.second.value_or("<none>"));
		std::println("unnamed : {}", parser.m_rawUnnamedArgs);
		return parser;
	}
}
