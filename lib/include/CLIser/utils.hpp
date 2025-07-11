#pragma once

#include <cmath>
#include <iterator>
#include <meta>
#include <optional>
#include <ranges>

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
		template <typename Range>
		class ChunkViewIterator final {
			using This = ChunkViewIterator<Range>;
			template <typename>
			friend class ChunkView;

			public:
				using difference_type = std::ptrdiff_t;
				constexpr ChunkViewIterator() noexcept = default;
				constexpr ChunkViewIterator(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr ChunkViewIterator(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto operator==(const This&) const noexcept -> bool = default;
				constexpr auto operator<=>(const This&) const noexcept = default;

				constexpr auto operator++() noexcept -> This& {
					++m_pos;
					return *this;
				}
				constexpr auto operator++(int) noexcept -> This {
					auto tmp {*this};
					++(*this);
					return tmp;
				}

				constexpr auto operator*() const noexcept {
					const std::size_t pos {m_pos * m_chunkSize};
					const std::size_t tailSize {m_rangeSize - pos};
					const std::size_t size {tailSize >= m_chunkSize ? m_chunkSize : tailSize};
					auto it {std::ranges::begin(*m_range)};
					std::ranges::advance(it, pos);
					return std::views::counted(it, size);
				}

			private:
				constexpr ChunkViewIterator(Range &range, std::size_t chunkSize, std::size_t pos) noexcept :
					m_range {&range},
					m_rangeSize {std::ranges::size(*m_range)},
					m_chunkSize {chunkSize},
					m_pos {pos}
				{}

				Range *m_range;
				std::size_t m_rangeSize;
				std::size_t m_chunkSize;
				std::size_t m_pos;
		};

		template <typename Range>
		class ChunkView final : std::ranges::view_interface<ChunkView<Range>> {
			using This = ChunkView<Range>;
			public:
				template <std::ranges::viewable_range Arg>
				constexpr ChunkView(std::size_t chunkSize, Arg&& arg) :
					m_chunkSize {chunkSize},
					m_range {std::forward<Arg> (arg)},
					m_rangeSize {std::ranges::size(m_range)},
					m_chunkCount {(std::size_t)std::ceil(m_rangeSize / (float)m_chunkSize)}
				{}

				constexpr ChunkView(const This&) noexcept = default;
				constexpr auto operator=(const This&) noexcept -> This& = default;
				constexpr ChunkView(This&&) noexcept = default;
				constexpr auto operator=(This&&) noexcept -> This& = default;

				constexpr auto begin() noexcept {
					return ChunkViewIterator{m_range, m_chunkSize, 0};
				}
				constexpr auto end() noexcept {
					return ChunkViewIterator{m_range, m_chunkSize, m_chunkCount};
				}

			private:
				std::size_t m_chunkSize;
				Range m_range;
				std::size_t m_rangeSize;
				std::size_t m_chunkCount;
		};

		template <std::ranges::viewable_range Arg>
		ChunkView(std::size_t, Arg&&) -> ChunkView<std::views::all_t<Arg>>;

		struct ChunkAdaptorClosure final : std::ranges::range_adaptor_closure<ChunkAdaptorClosure> {
			std::size_t m_chunkSize {};
			constexpr ChunkAdaptorClosure(std::size_t chunkSize) noexcept :
				std::ranges::range_adaptor_closure<ChunkAdaptorClosure> {},
				m_chunkSize {chunkSize}
			{}
			constexpr auto operator()(std::ranges::viewable_range auto&& range) const noexcept {
				return ChunkView{m_chunkSize, std::forward<decltype(range)> (range)};
			}
		};

		struct ChunkAdaptorClosureFactory final {
			constexpr auto operator()(std::size_t chunkSize) const noexcept {
				return ChunkAdaptorClosure{chunkSize};
			}
		};

		constexpr ChunkAdaptorClosureFactory chunk {};

		static_assert(std::input_or_output_iterator<ChunkViewIterator<std::string>>);
		static_assert(std::sentinel_for<ChunkViewIterator<std::string>, ChunkViewIterator<std::string>>);
		static_assert(std::ranges::viewable_range<ChunkView<std::ranges::ref_view<std::string>>>);
	#endif
	}
}
