#pragma once

#include <vector>
#include <span>
#include <string_view>

#include "CLIser/core.hpp"


namespace CLIser {
	class ArgumentBase;

	class Context {
		friend class ArgumentBase;
		public:
			struct CreateInfos {
				std::span<char* const> args;
			};

			Context() = delete;
			Context(const Context&) = delete;
			auto operator=(const Context&) -> Context& = delete;
			auto operator=(Context&&) -> Context& = delete;

			constexpr Context(Context&&) noexcept = default;

			CLIser_CORE static auto construct(const CreateInfos &createInfos) noexcept -> Context;

			[[nodiscard]]
			CLIser_CORE auto parse() noexcept -> CLIser::Result;
			inline auto getArgs() const noexcept -> std::span<const std::string_view> {return m_args;}


		private:
			CLIser_CORE auto addArgument(ArgumentBase &argument) noexcept -> void;
			constexpr Context(std::vector<std::string_view> &&args) noexcept :
				m_args {std::move(args)},
				m_arguments {}
			{}

			std::vector<std::string_view> m_args;
			std::vector<std::reference_wrapper<ArgumentBase>> m_arguments;
	};
}
