cmake_minimum_required(VERSION 3.31)
project(PolinomiMapi)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(src)
include_directories(samples)

add_executable(PolinomiMapi
        samples/main.cpp
        src/UnorderedMapVec.h
)
