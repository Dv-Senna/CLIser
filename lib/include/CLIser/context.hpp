#pragma once

#include <vector>
#include <span>
#include <string_view>

#include "CLIser/core.hpp"


namespace CLIser {
	class Context {
		public:
			struct CreateInfos {
				std::span<char* const> args;
			};

			constexpr Context() noexcept = delete;
			constexpr Context(const Context&) noexcept = delete;
			constexpr auto operator=(const Context&) noexcept -> Context& = delete;
			constexpr auto operator=(Context&&) noexcept -> Context& = delete;

			constexpr Context(Context&&) noexcept = default;

			CLIser_CORE static auto construct(const CreateInfos &createInfos) noexcept -> Context;

			inline auto getArgs() const noexcept -> std::span<const std::string_view> {return m_args;}


		private:
			constexpr Context(std::vector<std::string_view> &&args) noexcept : m_args {std::move(args)} {}

			std::vector<std::string_view> m_args;
	};
}
