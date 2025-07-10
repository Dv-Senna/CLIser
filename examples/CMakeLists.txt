# CLIser examples
file(GLOB_RECURSE CLISER_EXAMPLES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(EXAMPLE IN LISTS CLISER_EXAMPLES_SOURCES)
	get_filename_component(TARGET ${EXAMPLE} NAME_WE)
	add_executable(${TARGET} ${EXAMPLE})
	target_link_libraries(${TARGET} PRIVATE CLIser::CLIser)
	target_include_directories(${TARGET} PRIVATE ${CMAKE_BINARY_DIR}/generated/include)

	if (MSVC)
		target_compile_options(${TARGET} PRIVATE /W4)
	else()
		target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic)
	endif()
endforeach()
