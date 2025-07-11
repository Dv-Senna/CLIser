#pragma once

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


	namespace views {
	#if __cpp_lib_ranges_ch >= 202202L
		constexpr auto &chunk {std::views::chunk};
	#else
		template <std::ranges::forward_range Range>
		class ChunkViewIterator final {
			using This = ChunkViewIterator<Range>;
			template <std::ranges::forward_range>
			friend class ChunkView;

			public:
				using value_type = decltype(std::views::counted(
					std::declval<std::ranges::iterator_t<Range>> (),
					std::size_t{})
				);
				using difference_type = std::iter_difference_t<std::ranges::iterator_t<Range>>;

				constexpr ChunkViewIterator() noexcept = default;
				constexpr ChunkViewIterator(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;

				constexpr auto operator==(const This&) const noexcept -> bool = default;
				constexpr auto operator<=> (const This& other) const noexcept -> std::partial_ordering {
					if (m_range != other.m_range)
						return std::partial_ordering::unordered;
					if (m_chunkSize != other.m_chunkSize)
						return std::partial_ordering::unordered;
					return m_chunkIndex <=> other.m_chunkIndex;
				}

				constexpr auto operator++() noexcept -> This& {++m_chunkIndex; return *this;}
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

				constexpr auto operator[](difference_type n) const noexcept {return *(*this + n);}

				constexpr auto operator*() const noexcept {
					const std::size_t position {m_chunkIndex * m_chunkSize};
					const std::size_t tailSize {m_rangeSize - position};
					const std::size_t chunkSize {tailSize >= m_chunkSize ? m_chunkSize : tailSize};
					auto it {std::ranges::begin(*m_range)};
					std::advance(it, position);
					return std::views::counted(it, chunkSize);
				}


			private:
				constexpr ChunkViewIterator(Range &range, std::size_t chunkSize, std::size_t chunkIndex) noexcept :
					m_range {&range},
					m_rangeSize {std::ranges::size(range)},
					m_chunkIndex {chunkIndex},
					m_chunkSize {chunkSize}
				{}


				Range* m_range;
				std::size_t m_rangeSize;
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


		template <std::ranges::forward_range Range>
		class ChunkView final : std::ranges::view_interface<ChunkView<Range>> {
			using This = ChunkView<Range>;

			public:
				using iterator = ChunkViewIterator<Range>;

				template <std::ranges::input_range Arg>
				constexpr ChunkView(Arg &&arg, std::size_t chunkSize) noexcept :
					m_range {std::forward<Arg> (arg)},
					m_chunkSize {chunkSize},
					m_chunkCount {(std::size_t)std::ceil(std::ranges::size(m_range) / (float)chunkSize)}
				{}

				constexpr auto begin() noexcept {return iterator{m_range, m_chunkSize, 0};}
				constexpr auto end() noexcept {return iterator{m_range, m_chunkSize, m_chunkCount};}


			private:
				Range m_range;
				std::size_t m_chunkSize;
				std::size_t m_chunkCount;
		};

		template <std::ranges::forward_range Range>
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
