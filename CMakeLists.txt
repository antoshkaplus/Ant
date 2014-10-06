
cmake_minimum_required (VERSION 2.8.11)
project (ANT)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")


include_directories("./include")

file(GLOB_RECURSE ANT_SOURCE_FILES "./src/*.cpp")

add_library(ant ${ANT_SOURCE_FILES})

#file(GLOB_RECURSE TEST_SOURCE_FILES "./test/*.cpp")
file(GLOB TEST_SOURCE_FILES "./test/*.cpp")

add_executable(test ${TEST_SOURCE_FILES})