#pragma once

#include <meta>
#include <optional>

#include "CLIser/argsConfig.hpp"


namespace CLIser::utils {
	template <std::meta::info r, typename T>
	consteval auto hasAnnotation() noexcept -> bool {
		template for (constexpr auto a : std::define_static_array(annotations_of(r))) {
			if constexpr (type_of(a) == ^^T)
				return true;
		}
		return false;
	}

	template <std::meta::info r, template <auto...> typename T>
	consteval auto hasAnnotation() noexcept -> bool {
		template for (constexpr auto a : std::define_static_array(annotations_of(r))) {
			if constexpr (has_template_arguments(type_of(a)) && template_of(type_of(a)) == ^^T) {
				using AnnotationType = [:type_of(a):];
				return true;
			}
			else if constexpr (has_template_arguments(type_of(a))
				&& template_of(type_of(a)) == ^^CLIser::internals::OptionalWrapper
			) {
				constexpr auto inner {std::define_static_array(template_arguments_of(type_of(a)))[0]};
				if (inner == ^^T)
					return true;
			}
		}
		return false;
	}


	template <std::meta::info r, typename T>
	consteval auto getAnnotation() -> T {
		template for (constexpr auto a : std::define_static_array(annotations_of(r))) {
			if constexpr (type_of(a) == ^^T)
				return extract<T> (a);
		}
		throw "Can't get not present annotation. Please use `utils::hasAnnotation to check for presence`";
	}

	template <std::meta::info r, template <auto...> typename T>
	consteval auto getAnnotation() {
		template for (constexpr auto a : std::define_static_array(annotations_of(r))) {
			using AnnotationType = [:type_of(a):];
			if constexpr (has_template_arguments(type_of(a)) && template_of(type_of(a)) == ^^T) {
				return std::optional{extract<AnnotationType> (a)};
			}
			else if constexpr (has_template_arguments(type_of(a))
				&& template_of(type_of(a)) == ^^CLIser::internals::OptionalWrapper
			) {
				constexpr auto inner {std::define_static_array(template_arguments_of(type_of(a)))[0]};
				if constexpr (inner == ^^T)
					return std::optional<AnnotationType> {std::nullopt};
			}
		}
		throw "Can't get not present annotation. Please use `utils::hasAnnotation to check for presence`";
	}
}
