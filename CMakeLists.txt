cmake_minimum_required(VERSION 3.9)
project(kayovm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -pthread")

add_subdirectory(zlib)
add_subdirectory(vm)
add_subdirectory(test)

add_executable(kayovm jvm.cpp)

target_link_libraries(kayovm vmlib)
