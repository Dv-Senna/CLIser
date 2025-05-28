cmake_minimum_required(VERSION 3.25)


project(CLIser
	VERSION 0.0.1
	DESCRIPTION "CLI tools building library"
	LANGUAGES CXX
)


set(CLIser_LIB_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CLIser_SANDBOX_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/sandbox)

set(CLIser_CPP_STANDARD 26 CACHE STRING "The cpp version used by CLIser")
option(CLIser_SHARED_LIBRARY "" ON)
option(CLIser_BUILD_SANDBOX_APP "" OFF)
option(CLIser_USE_ADDRESS_SANITIZER "" ON)
option(CLIser_DISABLE_EXCEPTIONS "" ON)


add_subdirectory(${CLIser_LIB_FOLDER})
add_subdirectory(${CLIser_SANDBOX_FOLDER})
