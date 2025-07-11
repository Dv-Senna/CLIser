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
			const bool hasHelp {CLIser::utils::hasAnnotation<^^T, CLIser::_Help> ()};
			const bool hasName {CLIser::utils::hasAnnotation<^^T, CLIser::Name> ()};
			const bool hasVersion {CLIser::utils::hasAnnotation<^^T, CLIser::Version> ()};
			const bool hasVersionDescription {CLIser::utils::hasAnnotation<^^T, CLIser::VersionDescription> ()};

			if (!CLIser::utils::isTypeShortAllShort<T> ())
				throw "All your Short must be one character wide. If you need more space, use Long";
			if (!CLIser::utils::isTypeLongAllLong<T> ())
				throw "All your Long must be more than one character wide. If you want a single character, use Short";

			if (hasHelp && (
				CLIser::utils::hasTypeOption<T> ("h")
				|| CLIser::utils::hasTypeOption<T> ("help")
			))
				throw "You can't use '-h' or '--help' options when help menu is enable";

			if (hasName != hasVersion)
				throw "You must specify both Name and Version, or neither of those as application info";
			if (hasName && (
				CLIser::utils::hasTypeOption<T> ("v")
				|| CLIser::utils::hasTypeOption<T> ("version")
			))
				throw "You can't use '-v' or '--version' when version menu is enable";
			if (hasVersionDescription && !hasName)
				throw "You can't specify a VersionDescription if the version menu is not enabled";
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

			template <argument_list ArgumentList>
			auto m_printHelp() const noexcept -> void;
			template <argument_list ArgumentList>
			auto m_printVersion() const noexcept -> void;

			static auto s_printArgumentHelp(
				std::string_view option,
				std::optional<std::string_view> description,
				const _Help& help
			) noexcept -> void;

			std::string_view m_commandName;
			std::unordered_map<std::string_view, std::optional<std::string_view>> m_rawArgs;
			std::vector<std::string_view> m_rawUnnamedArgs;
	};
}

#include "CLIser/parser.inl"
