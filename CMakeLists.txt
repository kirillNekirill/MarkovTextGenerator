project(MarkovTextGenerator)
cmake_minimum_required(VERSION 3.16)

include_directories(${CMAKE_SOURCE_DIR}/Model)
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(Model)
add_subdirectory(Fitter)
add_subdirectory(Generator)