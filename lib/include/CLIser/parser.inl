#pragma once

#include "CLIser/parser.hpp"

#include <algorithm>
#include <meta>
#include <print>
#include <ranges>

#include "CLIser/argsConfig.hpp"
#include "CLIser/utils.hpp"


namespace CLIser {
	template <argument_list ArgumentList>
	auto Parser::parse() const noexcept -> std::expected<ArgumentList, std::string> {
		using namespace std::string_view_literals;
		constexpr auto ctx {std::meta::access_context::current()};
		constexpr auto members {std::define_static_array(nonstatic_data_members_of(^^ArgumentList, ctx))};


		if constexpr (CLIser::utils::hasAnnotation<^^ArgumentList, CLIser::_Help> ()) {
			const bool helpRequested {std::ranges::fold_left(std::array{"h"sv, "help"sv}
				| std::views::transform([this](const auto key) {
					return this->m_rawArgs.find(key) != this->m_rawArgs.end();
				}),
				false, std::logical_or<bool> {}
			)};
			if (helpRequested) {
				this->m_printHelp<ArgumentList> ();
				return ArgumentList{};
			}
		}


		template for (constexpr auto member : members) {
			std::size_t valueCount {0};
			std::optional<std::string_view> value {std::nullopt};

			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::Description> ()) {
				constexpr auto description {CLIser::utils::getAnnotation<member, CLIser::Description> ()};
				std::println("Description : {}", description->value);
			}
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Short> ()) {
				constexpr auto short_ {CLIser::utils::getAnnotation<member, CLIser::_Short> ()};
				if constexpr (!short_)
					std::println("Empty short");
				else
					std::println("Short : {}", short_->value);
			}
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Long> ()) {
				constexpr auto long_ {CLIser::utils::getAnnotation<member, CLIser::_Long> ()};
				if constexpr (!long_)
					std::println("Empty long");
				else
					std::println("Long : {}", long_->value);
			}
		}
		return ArgumentList{};
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

		template for (constexpr auto member : members) {
			std::string option {};
			std::optional<std::string_view> description {};

			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Short> ()) {
				constexpr auto short_ {CLIser::utils::getAnnotation<member, CLIser::_Short> ()};
				if constexpr (!short_)
					option += "-"s + identifier_of(member)[0];
				else
					option += "-"s + short_->value;
			}
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::_Long> ()) {
				if (!option.empty())
					option += ',';
				constexpr auto long_ {CLIser::utils::getAnnotation<member, CLIser::_Long> ()};
				if constexpr (!long_)
					option += "--"s + identifier_of(member);
				else
					option += "--"s + long_->value;
			}
			if constexpr (CLIser::utils::hasAnnotation<member, CLIser::Description> ())
				description = CLIser::utils::getAnnotation<member, CLIser::Description> ()->value;

			s_printArgumentHelp(option, description, help);
		}
	}
}
