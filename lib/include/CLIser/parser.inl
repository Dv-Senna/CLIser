#pragma once

#include "CLIser/parser.hpp"

#include <algorithm>
#include <charconv>
#include <meta>
#include <print>
#include <ranges>
#include <type_traits>

#include "CLIser/argsConfig.hpp"
#include "CLIser/utils.hpp"


namespace CLIser {
	template <argument_list ArgumentList>
	auto Parser::parse() const noexcept -> std::expected<ArgumentList, std::string> {
		using namespace std::string_view_literals;
		constexpr auto ctx {std::meta::access_context::current()};
		constexpr auto members {std::define_static_array(nonstatic_data_members_of(^^ArgumentList, ctx))};

		const auto hasOneOfArguments = [this](const auto& args) {
			return std::ranges::fold_left(args
				| std::views::transform([this](const auto key) {
					return this->m_rawArgs.find(key) != this->m_rawArgs.end();
				}),
				false, std::logical_or<bool> {}
			);
		};

		if constexpr (CLIser::utils::hasAnnotation<^^ArgumentList, CLIser::_Help> ()) {
			if (hasOneOfArguments(std::array{"h"sv, "help"sv})) {
				this->m_printHelp<ArgumentList> ();
				return ArgumentList{};
			}
		}

		if constexpr (CLIser::utils::hasAnnotation<^^ArgumentList, CLIser::Version> ()) {
			if (hasOneOfArguments(std::array{"v"sv, "version"sv})) {
				this->m_printVersion<ArgumentList> ();
				return ArgumentList{};
			}
		}

		ArgumentList result {};
		template for (constexpr auto member : members) {
			using namespace std::string_literals;
			std::vector<std::optional<std::string_view>> values {};
			std::vector<std::string> keys {};

			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Short> ())
				keys.push_back(CLIser::utils::getMemberShort<member> ());
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Long> ()) {
				keys.push_back(CLIser::utils::getMemberLong<member> ());
			}

			if (keys.empty())
				continue;

			std::string keyUsed {};
			for (const auto& key : keys) {
				auto it {m_rawArgs.find(key)};
				if (it == m_rawArgs.end())
					continue;
//				values.append_range(it->second);
				keyUsed = key;
				values.push_back(it->second);
			}

			if (keyUsed.empty())
				keyUsed = (keys[0].size() == 1 ? "-"s : "--"s) + keys[0];

			if (values.size() > 1) {
				return std::unexpected(
					"Multiple instance of the same argument in command line is not supported for now"
				);
			}

		#ifdef CLIser_PARSE_ERROR_STATEMENT
			#error Macro CLIser_PARSE_ERROR_STATEMENT is used by CLIser, so you must not define it
			#include <stop_compilation>
		#endif
		#define CLIser_PARSE_ERROR_STATEMENT std::unexpected(std::format("Value '{}' of argument {} can't be parsed",\
			*values[0], keyUsed))

			if constexpr (has_template_arguments(type_of(member)) && template_of(type_of(member)) == ^^std::optional) {
				if (values.empty() || !values[0])
					continue;
				auto parsed {this->[:substitute(^^Parser::m_parseString, {
					template_arguments_of(type_of(member))[0]
				}):] (*values[0])};
				if (!parsed)
					return CLIser_PARSE_ERROR_STATEMENT;
				result.[:member:] = *parsed;
			}
			else if constexpr (type_of(member) == ^^bool) {
				if (values.empty()) {
					result.[:member:] = false;
					continue;
				}
				if (!values[0]) {
					result.[:member:] = true;
					continue;
				}
				auto parsed {this->m_parseString<bool> (*values[0])};
				if (!parsed)
					return CLIser_PARSE_ERROR_STATEMENT;
				result.[:member:] = *parsed;
			}
			else {
				if (values.empty() || !values[0])
					return std::unexpected(std::format("Argument {} is mandatory", keyUsed));
				auto parsed {this->[:substitute(^^Parser::m_parseString, {type_of(member)}):] (*values[0])};
				if (!parsed)
					return CLIser_PARSE_ERROR_STATEMENT;
				result.[:member:] = *parsed;
			}
		#undef CLIser_PARSE_ERROR_STATEMENT
		}
		return result;
	}


	template <>
	auto Parser::m_parseString<std::string_view> (std::string_view) const noexcept -> std::optional<std::string_view>;
	template <>
	auto Parser::m_parseString<std::string> (std::string_view) const noexcept -> std::optional<std::string>;
	template <>
	auto Parser::m_parseString<bool> (std::string_view) const noexcept -> std::optional<bool>;

	template <typename T>
	auto Parser::m_parseString(std::string_view value) const noexcept -> std::optional<T> {
		if constexpr (std::is_arithmetic_v<T>) {
			T result {};
			const auto [ptr, ec] {std::from_chars(value.data(), value.data() + value.size(), result)};
			if (ec != std::errc{})
				return std::nullopt;
			return result;
		}
		else {
			consteval {throw "Given type can't be parsed";};
		}
	}


	template <argument_list ArgumentList>
	auto Parser::m_printHelp() const noexcept -> void {
		using namespace std::string_literals;
		constexpr auto ctx {std::meta::access_context::current()};
		constexpr auto members {std::define_static_array(nonstatic_data_members_of(^^ArgumentList, ctx))};
		constexpr auto help {CLIser::utils::getAnnotation<^^ArgumentList, CLIser::_Help> ()};

		std::println("Usage: {} [options]", m_commandName);
		std::println("Options:");
		s_printArgumentHelp("-h,--help", "Display this menu", help);

		if constexpr (CLIser::utils::hasAnnotation<^^ArgumentList, CLIser::Name> ())
			s_printArgumentHelp("-v,--version", "Display the version of the application", help);

		template for (constexpr auto member : members) {
			std::string option {};
			std::optional<std::string_view> description {};

			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Short> ()) {
				option += "-"s + CLIser::utils::getMemberShort<member> ();
			}
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Long> ()) {
				if (!option.empty())
					option += ',';
				option += "--"s + CLIser::utils::getMemberLong<member> ();
			}
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::Description> ())
				description = CLIser::utils::getAnnotation<member, CLIser::Description> ()->value;

			s_printArgumentHelp(option, description, help);
		}
	}


	template <argument_list ArgumentList>
	auto Parser::m_printVersion() const noexcept -> void {
		constexpr auto name {*CLIser::utils::getAnnotation<^^ArgumentList, CLIser::Name> ()};
		constexpr auto version {*CLIser::utils::getAnnotation<^^ArgumentList, CLIser::Version> ()};
		std::println("{} {}", name.value, version.value);
		if constexpr (CLIser::utils::hasAnnotation<^^ArgumentList, CLIser::VersionDescription> ()) {
			constexpr auto versionDescription {
				*CLIser::utils::getAnnotation<^^ArgumentList, CLIser::VersionDescription> ()
			};
			std::println("{}", versionDescription.value);
		}
	}
}
