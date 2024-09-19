#pragma once


#ifdef CLI_STATIC
	#define CLI_CORE
#elif defined(CLI_SHARED)
	#ifdef CLI_WINDOWS
		#ifdef CLI_BUILD_LIB
			#define CLI_CORE __declspec(dllexport)
		#else
			#define CLI_CORE __declspec(dllimport)
		#endif
	#else
		#define CLI_CORE
	#endif
#else
	#error You must define either 'CLI_STATIC' or 'TP_SHARED'
#endif