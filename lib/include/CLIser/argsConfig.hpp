#pragma once

#include <concepts>
#include <cstddef>
#include <meta>
#include <ranges>
#include <type_traits>
#include <utility>


namespace CLIser {
	namespace internals {
		template <typename T>
		struct is_string : std::false_type {};

		template <std::integral auto N>
		struct is_string<char[N]> : std::true_type {};

		template <typename T>
		concept string = is_string<std::remove_cvref_t<T>>::value;


		template <typename ToBuild, bool optional>
		struct StringAnnotation {
			using This = StringAnnotation<ToBuild, optional>;
			const char* value {nullptr};

			constexpr StringAnnotation() noexcept : value {nullptr} {}
			constexpr StringAnnotation(const char* value) noexcept : value {value} {}
			constexpr StringAnnotation(const This&) requires (optional) = default;
			constexpr StringAnnotation(const This& other) requires (!optional) {
				*this = other;
			}

			constexpr auto operator=(const This&) -> This& requires (optional) = default;
			constexpr auto operator=(const This& other) -> This& requires (!optional) {
				if consteval {
					if (other.value == nullptr)
						throw "Can't use non-optional string annotation without providing a value";
				}
				value = other.value;
				return *this;
			}
			constexpr ~StringAnnotation() = default;

			consteval auto operator()() const noexcept requires (optional) {
				return ToBuild{nullptr};
			}
			template <std::integral auto N>
			consteval auto operator()(const char (&value)[N]) const noexcept {
				return ToBuild{std::define_static_string(value)};
			}

			explicit constexpr operator bool() const noexcept requires (optional) {return value != nullptr;}
		};
	}

	namespace annotations {
		struct Description : internals::StringAnnotation<Description, false> {};
		struct Name : internals::StringAnnotation<Name, false> {};
		struct Version : internals::StringAnnotation<Version, false> {};
		struct VersionDescription : internals::StringAnnotation<VersionDescription, false> {};
		struct Short : internals::StringAnnotation<Short, true> {};
		struct Long : internals::StringAnnotation<Long, true> {};

		struct FatalUnknown {};
		struct SilenceUnknownWarnings {};
		struct Help {
			std::size_t tabulationSize {4};
			std::size_t descriptionAlignment {30};
			std::size_t maxDescriptionWidth {50};
			consteval auto operator()(auto&&... args) noexcept {return Help{std::forward<decltype(args)> (args)...};}
		};
	}

	constexpr annotations::Description Description {};
	constexpr annotations::Name Name {};
	constexpr annotations::Version Version {};
	constexpr annotations::VersionDescription VersionDescription {};
	constexpr annotations::Short Short {};
	constexpr annotations::Long Long {};
	constexpr annotations::FatalUnknown FatalUnknown {};
	constexpr annotations::SilenceUnknownWarnings SilenceUnknownWarnings {};
	constexpr annotations::Help Help {};
}
