#pragma once

#include <cstddef>
#include <ranges>


namespace CLIser {
	namespace internals {
		template <template <auto...> typename T>
		struct OptionalWrapper {
			constexpr OptionalWrapper() noexcept = default;
			template <typename ...Args>
			constexpr auto operator()(Args&&... args) const noexcept {
				return T{std::forward<Args> (args)...};
			}
		};
	}

#ifdef CLIser_MAKE_STRING_VALUE_ARG
	#error CLIser_MAKE_STRING_VALUE_ARG macro name must be available as CLIser use it
	#include <stop_compilation>
#endif
#define CLIser_MAKE_STRING_VALUE_ARG(name) template <std::size_t N>\
	struct name {\
		char value[N];\
		constexpr name(const char (&data)[N]) noexcept {\
			for (const auto i : std::views::iota(0uz, N))\
				value[i] = data[i];\
		}\
	}

#ifdef CLIser_MAKE_OPTIONAL_STRING_VALUE_ARG
	#error CLIser_MAKE_OPTIONAL_STRING_VALUE_ARG macro name must be available as CLIser use it
	#include <stop_compilation>
#endif
#define CLIser_MAKE_OPTIONAL_STRING_VALUE_ARG(name) CLIser_MAKE_STRING_VALUE_ARG(_##name);\
	constexpr ::CLIser::internals::OptionalWrapper<_##name> name {}


	CLIser_MAKE_STRING_VALUE_ARG(Description);
	CLIser_MAKE_OPTIONAL_STRING_VALUE_ARG(Short);
	CLIser_MAKE_OPTIONAL_STRING_VALUE_ARG(Long);
	CLIser_MAKE_STRING_VALUE_ARG(Name);
	CLIser_MAKE_STRING_VALUE_ARG(Version);
	struct _Help {};
	constexpr _Help Help {};

	static_assert(std::same_as<Description<6>, decltype(Description{"Hello"})>);
}

#undef CLIser_MAKE_OPTIONAL_STRING_VALUE_ARG 
#undef CLIser_MAKE_STRING_VALUE_ARG
