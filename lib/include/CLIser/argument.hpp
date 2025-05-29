#pragma once

#include <charconv>
#include <concepts>
#include <functional>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

#include "CLIser/context.hpp"
#include "CLIser/core.hpp"
#include "CLIser/traits.hpp"


namespace CLIser {
	template <typename T>
	struct is_argument : std::false_type {};

	template <CLIser::string T>
	struct is_argument<T> : std::true_type {};
	template <std::integral T>
	struct is_argument<T> : std::true_type {};
	template <std::floating_point T>
	struct is_argument<T> : std::true_type {};
	template <> struct is_argument<bool> : std::true_type {};

	template <typename T>
	constexpr auto is_argument_v = is_argument<T>::value;

	template <typename T>
	concept argument = is_argument_v<T>;


	class ArgumentBase {
		public:
			ArgumentBase() = delete;
			ArgumentBase(const ArgumentBase&) = delete;
			auto operator=(const ArgumentBase&) -> ArgumentBase& = delete;
			auto operator=(ArgumentBase&&) -> ArgumentBase& = delete;

			constexpr ArgumentBase(ArgumentBase&&) noexcept = default;
			virtual constexpr ~ArgumentBase() = default;

			constexpr auto getTags() const noexcept -> std::span<const std::string_view> {return m_tags;}
			virtual auto parse(std::string_view content) noexcept -> CLIser::Result = 0;

		protected:
			constexpr ArgumentBase(Context &context, std::initializer_list<std::string_view> tags) noexcept :
				m_tags {tags}
			{
				context.addArgument(*this);
			}
		private:
			std::vector<std::string_view> m_tags;
	};


	template <argument T>
	class Argument final : public ArgumentBase {
		public:
			using Type = std::conditional_t<CLIser::string<T>, std::string_view, T>;

		private:
			struct ArrowOperatorProxy {
				Type value;
				inline auto operator->() noexcept -> Type* {return &value;}
				inline auto operator->() const noexcept -> const Type* {return &value;}
			};

		public:
			struct CreateInfos {
				Context &context;
				std::initializer_list<std::string_view> tags;
				std::string_view description;
				std::optional<Type> defaultValue {std::nullopt};
				std::move_only_function<bool(const T&)> condition {[](const T&){return true;}};
			};

			Argument() = delete;

			constexpr Argument(Argument<T>&&) noexcept = default;
			constexpr ~Argument() override = default;

			[[nodiscard]]
			static constexpr auto construct(CreateInfos &&createInfos) noexcept -> Argument<T> {
				return Argument<T> {std::move(createInfos)};
			}

			[[nodiscard]]
			auto parse(std::string_view content) noexcept -> CLIser::Result override {
				if constexpr (CLIser::string<T>)
					m_value = content;
				else {
					T value {};
					const auto [ptr, ec] {std::from_chars(content.begin(), content.end(), value)};
					if (ec != std::errc{})
						return CLIser::Result::eNumberArgumentParseFailure;
					if (ptr != content.end())
						return CLIser::Result::eNumberArgumentParseFailure;
					m_value = value;
				}
				if (!m_condition(T{*m_value}))
					return CLIser::Result::eArgumentConditionFailure;
				return CLIser::Result::eSuccess;
			}

			inline auto hasValue() const noexcept -> bool {
				return !!m_value;
			}
			inline explicit operator bool() const noexcept {
				return this->hasValue();
			}
			inline auto getValue() const noexcept -> std::optional<Type> {
				return m_value.or_else([this](){return m_defaultValue;});
			}
			inline auto operator*() const noexcept -> Type {
				return *this->getValue();
			}
			inline auto operator->() const noexcept -> ArrowOperatorProxy {
				return ArrowOperatorProxy{**this};
			}


		private:
			constexpr Argument(CreateInfos &&createInfos) noexcept :
				ArgumentBase(createInfos.context, createInfos.tags),
				m_description {createInfos.description},
				m_defaultValue {createInfos.defaultValue},
				m_value {std::nullopt},
				m_condition {std::move(createInfos.condition)}
			{}
			std::string_view m_description;
			std::optional<Type> m_defaultValue;
			std::optional<Type> m_value;
			std::move_only_function<bool(const T&)> m_condition;
	};
}
