cmake_minimum_required(VERSION 3.25)


set(CLIser_SANDBOX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CLIser_SANDBOX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


file(GLOB_RECURSE CLIser_SANDBOX_SOURCE_FILES ${CLIser_SANDBOX_SOURCE_DIR}/*.cpp)

add_executable(CLIser_sandbox ${CLIser_SANDBOX_SOURCE_FILES})
set_property(TARGET CLIser_sandbox PROPERTY CXX_STANDARD ${CLIser_CPP_STANDARD})
set_property(TARGET CLIser_sandbox PROPERTY EXCLUDE_FROM_ALL NOT ${CLIser_BUILD_SANDBOX_APP})

target_include_directories(CLIser_sandbox PRIVATE ${CLIser_SANDBOX_INCLUDE_DIR})
target_link_libraries(CLIser_sandbox PRIVATE CLIser::CLIser)
