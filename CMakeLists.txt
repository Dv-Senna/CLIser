cmake_minimum_required(VERSION 3.30)

project(CLIser
	VERSION 0.1.0
	DESCRIPTION "CLI tools building library"
	LANGUAGES CXX
)

include(GenerateExportHeader)



# CLIser lib
file(GLOB_RECURSE CLISER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/*.cpp)
add_library(CLIser SHARED ${CLISER_SOURCES})
add_library(CLIser::CLIser ALIAS CLIser)

set(GENERATE_EXPORT_HEADERS_UNDEF_DEPRECATION "
#undef CLISER_DEPRECATED
#undef CLISER_DEPRECATED_EXPORT
#undef CLISER_DEPRECATED_NO_EXPORT"
)
generate_export_header(CLIser
	EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/generated/include/CLIser/export.hpp
	BASE_NAME CLIser
	EXPORT_MACRO_NAME CLIser_EXPORT
	NO_EXPORT_MACRO_NAME CLIser_NO_EXPORT
	STATIC_DEFINE CLIser_STATIC_DEFINE
	CUSTOM_CONTENT_FROM_VARIABLE GENERATE_EXPORT_HEADERS_UNDEF_DEPRECATION
)

target_include_directories(CLIser
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/lib/include
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/generated/include
)
target_compile_features(CLIser PUBLIC cxx_std_26)

if (MSVC)
	target_compile_options(CLIser PRIVATE /W4)
else()
	target_compile_options(CLIser PRIVATE -Wall -Wextra -Wpedantic)
endif()


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
