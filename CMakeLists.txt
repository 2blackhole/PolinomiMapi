cmake_minimum_required(VERSION 3.14)
project(PolinomiMapi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

# ----- Google Test -----
include(FetchContent)
FetchContent_Declare(
    googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip"
)

# Для Windows: отключаем параллельную сборку googletest (иногда помогает)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

#enable_testing()

# Find all test files in the 'tests' directory
file(GLOB TEST_SOURCES "tests/*.cpp")

# Create a separate test executable
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests gtest_main gtest)

# Register tests with CTest
#include(GoogleTest)
#gtest_discover_tests(run_tests)