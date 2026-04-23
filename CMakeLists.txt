cmake_minimum_required(VERSION 3.16)
project(PolinomiMapi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(polinoms_lib INTERFACE)
target_include_directories(polinoms_lib INTERFACE src)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL "https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip"
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    file(GLOB TEST_SOURCES "tests/*.cpp")
    add_executable(run_tests ${TEST_SOURCES})
    target_link_libraries(run_tests gtest_main gtest polinoms_lib)
endif()

option(BUILD_QT_GUI "Build Qt GUI" ON)
if(BUILD_QT_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    add_subdirectory(qt_gui)
endif()