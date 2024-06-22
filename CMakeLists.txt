cmake_minimum_required(VERSION 3.28)

project(scc CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)

include_directories(.)

add_subdirectory(src)