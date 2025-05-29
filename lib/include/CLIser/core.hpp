#pragma once

#include <cinttypes>
#include <optional>
#include <utility>


#ifdef CLIser_BUILD_SHARED
	#ifdef _WIN32
		#ifdef CLIser_BUILD_LIB
			#define CLIser_CORE __declspec(dllexport)
		#else
			#define CLIser_CORE __declspec(dllimport)
		#endif
	#else
		#define CLIser_CORE
	#endif
#else
	#define CLIser_CORE
#endif


namespace CLIser {
	enum class Result : std::int32_t {
		eSuccess = 0,
		eFailure = -1,
		eNumberArgumentParseFailure = -2,
		eArgumentConditionFailure = -3,
	};

	constexpr auto operator!(Result result) noexcept -> bool {
		return std::to_underlying(result) < 0;
	}
}
