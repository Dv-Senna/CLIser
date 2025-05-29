#pragma once

#include <string>
#include <string_view>
#include <type_traits>


namespace CLIser {
	template <typename T>
	struct is_string : std::false_type {};

	template <std::integral auto N>
	struct is_string<char[N]> : std::true_type {};
	template <std::integral auto N>
	struct is_string<const char[N]> : std::true_type {};
	template <> struct is_string<char*> : std::true_type {};
	template <> struct is_string<const char*> : std::true_type {};
	template <> struct is_string<std::string> : std::true_type {};
	template <> struct is_string<std::string_view> : std::true_type {};

	template <typename T>
	constexpr auto is_string_v = is_string<T>::value;

	template <typename T>
	concept string = is_string_v<T>;
}
