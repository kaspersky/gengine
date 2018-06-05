cmake_minimum_required(VERSION 2.6)

project(main)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O3")

add_subdirectory(src)
add_subdirectory(src/uttt)
add_subdirectory(src/ttt)
add_subdirectory(src/manager)
