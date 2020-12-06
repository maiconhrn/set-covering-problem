cmake_minimum_required(VERSION 3.17)
project(set_covering_problem)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(build)
include_directories(src)

add_executable(set_covering_problem
        src/main.cpp src/scp.hpp src/scp.cpp)
