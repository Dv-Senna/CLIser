include(GenerateExportHeader)

# CLIser lib
file(GLOB_RECURSE CLISER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE CLISER_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
add_library(CLIser SHARED ${CLISER_SOURCES})
add_library(CLIser::CLIser ALIAS CLIser)

set(GENERATE_EXPORT_HEADERS_UNDEF_DEPRECATION "
#undef CLISER_DEPRECATED
#undef CLISER_DEPRECATED_EXPORT
#undef CLISER_DEPRECATED_NO_EXPORT"
)
generate_export_header(CLIser
	EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/generated/include/CLIser/export.hpp
	BASE_NAME CLIser
	EXPORT_MACRO_NAME CLIser_EXPORT
	NO_EXPORT_MACRO_NAME CLIser_NO_EXPORT
	STATIC_DEFINE CLIser_STATIC_DEFINE
	CUSTOM_CONTENT_FROM_VARIABLE GENERATE_EXPORT_HEADERS_UNDEF_DEPRECATION
)

target_include_directories(CLIser
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)
target_compile_features(CLIser PUBLIC cxx_std_26)

if (MSVC)
	target_compile_options(CLIser PRIVATE /W4)
else()
	target_compile_options(CLIser PRIVATE -Wall -Wextra -Wpedantic)
endif()


install(TARGETS CLIser EXPORT CLIserTargets
	LIBRARY  DESTINATION lib
	ARCHIVE  DESTINATION lib
	RUNTIME  DESTINATION bin
	INCLUDES DESTINATION include
)

export(EXPORT CLIserTargets 
	FILE "${CMAKE_CURRENT_BINARY_DIR}/CLIserTargets.cmake"
	NAMESPACE CLIser::
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/CLIserConfigVersion.cmake"
	VERSION ${CLIser_VERSION}
	COMPATIBILITY SameMinorVersion
)
configure_file(${PROJECT_SOURCE_DIR}/cmake/CLIserConfig.cmake
	"${CMAKE_CURRENT_BINARY_DIR}/CLIserConfig.cmake"
	COPYONLY
)

install(EXPORT CLIserTargets
	FILE CLIserTargets.cmake
	NAMESPACE CLIser::
	DESTINATION lib/cmake/CLIser
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/CLIserConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/CLIserConfigVersion.cmake"
	DESTINATION lib/cmake/CLIser
	COMPONENT DEVEL
)

install(
	FILES
		${PROJECT_BINARY_DIR}/generated/include/CLIser/export.hpp
		${CLISER_HEADERS}
	DESTINATION include/CLIser
	COMPONENT DEVEL
)

#install(FILES ${PROJECT_BINARY_DIR}/generated/include/CLIser/export.hpp
#	DESTINATION ${CMAKE_INSTALL_PREFIX} 
#)]
