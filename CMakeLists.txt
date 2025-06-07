
cmake_minimum_required(VERSION 3.23)

project(config LANGUAGES CXX)

add_subdirectory(module/core)
add_subdirectory(module/format)
add_subdirectory(module/io)

add_subdirectory(lib)
