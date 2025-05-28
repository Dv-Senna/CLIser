#include "CLIser/context.hpp"

#include <ranges>


namespace CLIser {
	auto Context::construct(const CreateInfos &createInfos) noexcept -> Context {
		auto args {createInfos.args.subspan(1)
			| std::views::transform([](const auto &str) {return std::string_view{str};})
			| std::ranges::to<std::vector> ()
		};
		Context context {std::move(args)};
		return context;
	}
}
