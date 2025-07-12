cmake_minimum_required(VERSION 3.30)

project(CLIser
	VERSION 0.1.1
	DESCRIPTION "CLI tools building library"
	LANGUAGES CXX
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
