#pragma once

#include <optional>


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
