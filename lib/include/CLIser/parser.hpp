#pragma once

#include <expected>
#include <meta>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "CLIser/argsConfig.hpp"
#include "CLIser/export.hpp"
#include "CLIser/utils.hpp"


namespace CLIser {
	namespace internals {
		template <typename T>
		consteval auto isArgumentListValidAnnotations() {
			bool hasHelp {CLIser::utils::hasAnnotation<^^T, CLIser::_Help> ()};
			bool hasName {CLIser::utils::hasAnnotation<^^T, CLIser::Name> ()};
			bool hasVersion {CLIser::utils::hasAnnotation<^^T, CLIser::Version> ()};

			if (hasHelp && (!hasName || !hasVersion))
				throw "If Help is specified, you must also specify Name and Version of the application";
			return true;
		};
	}

	template <typename T>
	concept argument_list = std::is_aggregate_v<T> && internals::isArgumentListValidAnnotations<T> ();

	class CLIser_EXPORT Parser final {
		Parser(const Parser&) = delete;
		auto operator=(const Parser&) -> Parser& = delete;
		auto operator=(Parser&&) -> Parser& = delete;

		public:
			Parser(Parser &&parser) noexcept = default;
			~Parser() = default;

			struct CreateInfos {
				std::span<char* const> args;
			};

			[[nodiscard]]
			static auto create(CreateInfos &&createInfos) noexcept -> std::expected<Parser, std::string>;

			template <argument_list ArgumentList>
			auto parse() const noexcept -> std::expected<ArgumentList, std::string>;


		private:
			Parser() noexcept = default;

			std::unordered_map<std::string_view, std::optional<std::string_view>> m_rawArgs;
			std::vector<std::string_view> m_rawUnnamedArgs;
	};
}

#include "CLIser/parser.inl"
