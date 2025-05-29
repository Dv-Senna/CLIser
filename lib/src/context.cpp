#include "CLIser/context.hpp"

#include <print>
#include <ranges>

#include "CLIser/argument.hpp"


namespace CLIser {
	auto Context::construct(const CreateInfos &createInfos) noexcept -> Context {
		auto args {createInfos.args.subspan(1)
			| std::views::transform([](const auto &str) {return std::string_view{str};})
			| std::ranges::to<std::vector> ()
		};
		Context context {std::move(args)};
		return context;
	}


	auto Context::parse() noexcept -> CLIser::Result {
		auto cleanupValidArgs {m_args
			| std::views::transform([](const std::string_view arg) {
				const auto equal {arg.find_first_of('=')};
				std::string_view content {};
				if (equal != arg.size())
					content = {arg.begin() + equal + 1, arg.end()};
				const auto dashCount {arg.find_first_not_of('-')};
				return std::tuple{
					dashCount,
					std::string_view{arg.begin() + dashCount, equal - dashCount},
					content
				};
			})
			| std::views::filter([](const std::tuple<std::size_t, std::string_view, std::string_view> &arg) {
				const auto [dashCount, tag, _] = arg;
				if (tag.empty())
					return false;
				if (tag.size() == 1 && dashCount != 1)
					return false;
				if (tag.size() > 1 && dashCount != 2)
					return false;
				return true;
			})
			| std::views::transform([](const std::tuple<std::size_t, std::string_view, std::string_view> &arg) {
				const auto [_, tag, content] = arg;
				return std::pair{tag, content};
			})
			| std::ranges::to<std::vector> ()
		};

		auto argumentWithValue {m_arguments
			| std::views::transform([&cleanupValidArgs](ArgumentBase &argument) {
				std::optional<std::string_view> content {};
				for (const auto &tag : argument.getTags()) {
					auto it {std::ranges::find_if(cleanupValidArgs, [&tag](const auto &arg) {return arg.first == tag;})};
					if (it == cleanupValidArgs.end())
						continue;
					if (!!content)
						return std::pair{std::reference_wrapper{argument}, std::optional<std::string_view> {std::nullopt}};
					content = it->second;
				}
				return std::pair{std::reference_wrapper{argument}, content};
			})
			| std::views::filter([](const auto &argument) {return !!argument.second;})
			| std::views::transform([](const auto &argument) {
				return std::pair{argument.first, *argument.second};
			})
		};

		for (const auto &argument : argumentWithValue) {
			const CLIser::Result res {argument.first.get().parse(argument.second)};
			if (!res)
				return res;
		}
		return CLIser::Result::eSuccess;
	}


	auto Context::addArgument(ArgumentBase &argument) noexcept -> void {
		m_arguments.push_back(argument);
	}
}
