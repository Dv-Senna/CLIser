#pragma once

#include <cassert>
#include <cmath>
#include <compare>
#include <iterator>
#include <meta>
#include <optional>
#include <ranges>
#include <utility>

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


	template <std::meta::info r>
	consteval auto getMemberShort() -> std::string {
		if constexpr (!hasAnnotation<r, CLIser::_Short> ())
			throw "Can't get member short as it does not have this annotation";
		constexpr auto short_ {getAnnotation<r, CLIser::_Short> ()};
		if constexpr (!!short_)
			return std::string{short_->value};
		else
			return std::string{identifier_of(r)}.substr(0, 1);
	};

	template <std::meta::info r>
	consteval auto getMemberLong() -> std::string {
		if constexpr (!hasAnnotation<r, CLIser::_Long> ())
			throw "Can't get member long as it does not have this annotation";
		constexpr auto long_ {getAnnotation<r, CLIser::_Long> ()};
		if constexpr (!!long_)
			return std::string{long_->value};
		else
			return std::string{identifier_of(r)};
	};


	template <typename T>
	consteval auto hasTypeOption(std::string_view option) -> bool {
		constexpr auto ctx {std::meta::access_context::current()};
		constexpr auto members {std::define_static_array(nonstatic_data_members_of(^^T, ctx))};

		std::size_t count {};
		template for (constexpr auto member : members) {
			if constexpr (hasAnnotation<member, CLIser::_Short> ()) {
				if (getMemberShort<member> () == option)
					++count;
			}
			else if constexpr (hasAnnotation<member, CLIser::_Long> ()) {
				if (getMemberLong<member> () == option)
					++count;
			}
		}

		if (count > 1)
			throw "An option was supplied multiple times";
		return count == 1;
	}


	template <typename T>
	consteval auto isTypeShortAllShort() -> bool {
		constexpr auto ctx {std::meta::access_context::current()};
		constexpr auto members {std::define_static_array(nonstatic_data_members_of(^^T, ctx))};

		template for (constexpr auto member : members) {
			if constexpr (hasAnnotation<member, CLIser::_Short> ()) {
				if (getMemberShort<member> ().size() != 1)
					return false;
			}
		}
		return true;
	}

	template <typename T>
	consteval auto isTypeLongAllLong() -> bool {
		constexpr auto ctx {std::meta::access_context::current()};
		constexpr auto members {std::define_static_array(nonstatic_data_members_of(^^T, ctx))};

		template for (constexpr auto member : members) {
			if constexpr (hasAnnotation<member, CLIser::_Long> ()) {
				if (getMemberLong<member> ().size() <= 1)
					return false;
			}
		}
		return true;
	}


	namespace views {
	#if __cpp_lib_ranges_ch >= 202202L
		constexpr auto &chunk {std::views::chunk};
	#else
		template <std::ranges::input_range Range>
		class ChunkViewIterator final {
			using This = ChunkViewIterator<Range>;
			template <std::ranges::input_range>
			friend class ChunkView;

			public:
				using value_type = std::conditional_t<std::ranges::forward_range<Range>,
					decltype(std::ranges::subrange(
						std::declval<std::ranges::iterator_t<Range>> (), std::declval<std::ranges::iterator_t<Range>> ()
					) | std::views::take(std::size_t{})),
					decltype(std::declval<Range> () | std::views::take(std::size_t{}))
				>;
				using difference_type = std::iter_difference_t<std::ranges::iterator_t<Range>>;

				constexpr ChunkViewIterator() noexcept :
					m_range {nullptr},
					m_chunkIndex {0},
					m_chunkSize {0}
				{}
				constexpr ChunkViewIterator(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;

				constexpr auto operator==(const This& other) const noexcept -> bool {
					if (m_range == nullptr && other.m_range == nullptr)
						return true;
					if (m_range == nullptr || other.m_range == nullptr)
						return false;
					assert(m_range == other.m_range && "Can't compare iterator that are not on the same range");
					assert(m_chunkSize == other.m_chunkSize
						&& "Can't compare iterator that don't have the same chunk size"
					);
					return m_chunkIndex == other.m_chunkIndex;
				}
				constexpr auto operator<=> (const This& other) const noexcept -> std::partial_ordering {
					if (m_range != other.m_range)
						return std::partial_ordering::unordered;
					if (m_chunkSize != other.m_chunkSize)
						return std::partial_ordering::unordered;
					return m_chunkIndex <=> other.m_chunkIndex;
				}

				constexpr auto operator++() noexcept -> This& {
					if constexpr (std::ranges::forward_range<Range>) {
						++m_chunkIndex;
						const auto chunkCount {static_cast<std::size_t> (
							std::ceil(std::ranges::size(*m_range) / static_cast<float> (m_chunkSize))
						)};
						if (m_chunkIndex >= chunkCount)
							m_range = nullptr;
					}
					else {
						if constexpr (std::ranges::empty(*m_range))
							m_range = nullptr;
					}
					return *this;
				}
				constexpr auto operator++(int) noexcept -> This {auto tmp {*this}; ++(*this); return tmp;}
				constexpr auto operator--() noexcept -> This& {--m_chunkIndex; return *this;}
				constexpr auto operator--(int) noexcept -> This {auto tmp {*this}; --(*this); return tmp;}

				constexpr auto operator+=(difference_type n) noexcept -> This& {m_chunkIndex += n; return *this;}
				constexpr auto operator-=(difference_type n) noexcept -> This& {m_chunkIndex += n; return *this;}
				constexpr auto operator+(difference_type n) const noexcept -> This {auto tmp {*this}; return tmp += n;}
				constexpr auto operator-(difference_type n) const noexcept -> This {auto tmp {*this}; return tmp -= n;}

				constexpr auto operator-(const This& other) const noexcept -> difference_type {
					return m_chunkIndex - other.m_chunkIndex;
				}

				constexpr auto operator[](difference_type n) const noexcept -> value_type {return *(*this + n);}

				constexpr auto operator*() const noexcept {
					if constexpr (std::ranges::forward_range<Range>) {
						const std::size_t position {m_chunkIndex * m_chunkSize};
						auto it {std::ranges::begin(*m_range)};
						const auto sentinel {std::ranges::end(*m_range)};

						std::ranges::advance(it, position, sentinel);
						return std::ranges::subrange(it, sentinel) | std::views::take(m_chunkSize);
					}
					else
						return *m_range | std::views::take(m_chunkSize);
				}


			private:
				constexpr ChunkViewIterator(Range &range, std::size_t chunkSize) noexcept :
					m_range {&range},
					m_chunkIndex {0},
					m_chunkSize {chunkSize}
				{}


				Range* m_range;
				std::size_t m_chunkIndex;
				std::size_t m_chunkSize;
		};

		template <typename Range>
		constexpr auto operator+(
			std::iter_difference_t<ChunkViewIterator<Range>> n,
			ChunkViewIterator<Range> it
		) noexcept {
			return it += n;
		}

		static_assert(std::random_access_iterator<ChunkViewIterator<std::string>>);


		template <std::ranges::input_range Range>
		class ChunkView final : std::ranges::view_interface<ChunkView<Range>> {
			using This = ChunkView<Range>;

			public:
				using iterator = ChunkViewIterator<Range>;

				template <std::ranges::input_range _Range>
				constexpr ChunkView(_Range &&range, std::size_t chunkSize) noexcept :
					m_range {std::forward<_Range> (range)},
					m_chunkSize {chunkSize}
				{}

				constexpr auto begin() noexcept {return iterator{m_range, m_chunkSize};}
				constexpr auto end() noexcept {return iterator{};}


			private:
				Range m_range;
				std::size_t m_chunkSize;
		};

		template <std::ranges::input_range Range>
		ChunkView(Range&&, std::size_t) -> ChunkView<std::views::all_t<Range>>;

		static_assert(std::ranges::random_access_range<ChunkView<std::string>>);


		struct ChunkAdaptorClosure final : std::ranges::range_adaptor_closure<ChunkAdaptorClosure> {
			std::size_t chunkSize;
			constexpr ChunkAdaptorClosure(std::size_t chunkSize) noexcept :
				std::ranges::range_adaptor_closure<ChunkAdaptorClosure> {},
				chunkSize {chunkSize}
			{}

			[[nodiscard]]
			constexpr auto operator()(std::ranges::forward_range auto&& range) const noexcept {
				return ChunkView{std::forward<decltype(range)> (range), chunkSize};
			}
		};

		struct Chunk final {
			[[nodiscard]]
			constexpr auto operator()(std::size_t chunkSize) const noexcept {
				return ChunkAdaptorClosure{chunkSize};
			}
		};

		constexpr Chunk chunk {};
	#endif
	}
}
