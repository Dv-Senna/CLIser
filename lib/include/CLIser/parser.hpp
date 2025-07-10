#pragma once

#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

#include "CLIser/export.hpp"


namespace CLIser {
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


		private:
			Parser() noexcept = default;

			std::unordered_map<std::string_view, std::optional<std::string_view>> m_rawArgs;
	};
}
