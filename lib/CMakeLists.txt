cmake_minimum_required(VERSION 3.25)


set(CLIser_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CLIser_LIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


# interface to the headers of CLIser
add_library(CLIser_headers INTERFACE)
set_property(TARGET CLIser_headers PROPERTY CXX_STANDARD ${CLIser_CPP_STANDARD})

target_include_directories(CLIser_headers INTERFACE ${CLIser_LIB_INCLUDE_DIR})

target_compile_definitions(CLIser_headers INTERFACE CLIser_VERSION_MAJOR=${CLIser_VERSION_MAJOR})
target_compile_definitions(CLIser_headers INTERFACE CLIser_VERSION_MINOR=${CLIser_VERSION_MINOR})
target_compile_definitions(CLIser_headers INTERFACE CLIser_VERSION_PATCH=${CLIser_VERSION_PATCH})
if (${CLIser_SHARED_LIBRARY})
	target_compile_definitions(CLIser_headers INTERFACE CLIser_BUILD_SHARED)
endif()

# setup compiler options for development
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	if (MSVC)
		target_compile_options(CLIser_headers INTERFACE /W4)
	else()
		target_compile_options(CLIser_headers INTERFACE -Wall -Wextra -Wpedantic)
		if (CLIser_USE_ADDRESS_SANITIZER)
			target_compile_options(CLIser_headers INTERFACE -fsanitize=address)
			target_link_options(CLIser_headers INTERFACE -fsanitize=address)
		endif()
	endif()
endif()

# disable exceptions
if (CLIser_DISABLE_EXCEPTIONS)
	target_compile_definitions(CLIser_headers INTERFACE CLIser_DISABLE_EXCEPTIONS)
	if (MSVC)
		target_compile_options(CLIser_headers INTERFACE /EHsc)
	else()
		target_compile_options(CLIser_headers INTERFACE -fno-exceptions)
	endif()
endif()

add_library(CLIser::headers ALIAS CLIser_headers)


# some stuffs related to the library
file(GLOB_RECURSE CLIser_LIB_SOURCE_FILES ${CLIser_LIB_SOURCE_DIR}/*.cpp)


# static library of CLIser
add_library(CLIser_static_lib STATIC ${CLIser_LIB_SOURCE_FILES})
set_property(TARGET CLIser_static_lib PROPERTY CXX_STANDARD ${CLIser_CPP_STANDARD})
set_property(TARGET CLIser_static_lib PROPERTY EXCLUDE_FROM_ALL ${CLIser_SHARED_LIBRARY})

target_link_libraries(CLIser_static_lib PUBLIC CLIser::headers)

add_library(CLIser::static ALIAS CLIser_static_lib)


# shared library of CLIser
add_library(CLIser_shared_lib SHARED ${CLIser_LIB_SOURCE_FILES})
set_property(TARGET CLIser_shared_lib PROPERTY CXX_STANDARD ${CLIser_CPP_STANDARD})
set_property(TARGET CLIser_shared_lib PROPERTY EXCLUDE_FROM_ALL NOT ${CLIser_SHARED_LIBRARY})

target_compile_definitions(CLIser_shared_lib PRIVATE CLIser_BUILD_LIB)
target_link_libraries(CLIser_shared_lib PUBLIC CLIser::headers)

add_library(CLIser::shared ALIAS CLIser_shared_lib)
if (${CLIser_SHARED_LIBRARY})
	add_library(CLIser::CLIser ALIAS CLIser_shared_lib)
else()
	add_library(CLIser::CLIser ALIAS CLIser_static_lib)
endif()


