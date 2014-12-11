#
# test is actually depends on google test
#

# macros
macro(create_source_groups curdir)
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach(child ${children})
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        create_source_groups(${curdir}/${child})
    else()
        string(REPLACE "/" "\\" groupname ${curdir})
        source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
    endif()
    endforeach()
endmacro()

# body
cmake_minimum_required (VERSION 2.8.11)
project (ANT)

list(APPEND CMAKE_CXX_FLAGS "-std=c++1y")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -O3")



include_directories("./include")
include_directories("/usr/local/lib")

file(GLOB_RECURSE ANT_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ANT_HEADER_FILES "./include/*.h")

create_source_groups(src)
create_source_groups(include)

add_library(ant ${ANT_SOURCE_FILES} ${ANT_HEADER_FILES})




file(GLOB_RECURSE TEST_SOURCE_FILES "./test/*.cpp")
create_source_groups(test)
add_executable(test ${TEST_SOURCE_FILES})

# adding google test to 'test' executable
target_include_directories(test PUBLIC "/usr/local/include")
find_library(GTEST_MAIN_LIBRARY gtest_main)
find_library(GTEST_LIBRARY gtest)
target_link_libraries(test ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY} ant)

