cmake_minimum_required(VERSION 3.10.0)
project(powell-cat)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log_setup log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# If refactor makes it necessary, update name and srcs
add_library(session src/session.cc)
add_library(server src/server.cc)
add_library(config_parser src/config_parser.cc)
add_library(request_handler src/request_handler.cc)
add_library(echo_request_handler src/echo_request_handler.cc)
add_library(static_request_handler src/static_request_handler.cc)
add_library(error_handler src/error_handler.cc)

# If refactor makes it necessary, update deps
add_executable(webserver src/server_main.cc)
target_link_libraries(webserver server session config_parser request_handler echo_request_handler static_request_handler error_handler Boost::system Boost::log_setup Boost::log pthread)

# Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser static_request_handler request_handler echo_request_handler error_handler gtest_main Boost::log_setup Boost::log)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session config_parser request_handler echo_request_handler static_request_handler error_handler gtest_main Boost::log_setup Boost::log)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server session config_parser request_handler echo_request_handler static_request_handler error_handler gtest_main gmock_main Boost::log_setup Boost::log)

add_executable(request_handler_test tests/request_handler_test.cc)
target_link_libraries(request_handler_test config_parser request_handler echo_request_handler static_request_handler error_handler gtest_main Boost::log_setup Boost::log)

# Update with test binary
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Integration Test
add_test(
    NAME integration_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh ${CMAKE_BINARY_DIR}/bin/webserver
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS webserver session server config_parser request_handler echo_request_handler static_request_handler error_handler TESTS config_parser_test session_test server_test request_handler_test)
