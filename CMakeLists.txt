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


set(BinDir ${PROJECT_SOURCE_DIR}/bin)
set(LibDir ${PROJECT_SOURCE_DIR}/lib)

# archive is for static libraries
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinDir} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LibDir} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LibDir} )
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BinDir} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LibDir} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LibDir} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )



include_directories("./include")
include_directories("/usr/local/lib")


# ant library

file(GLOB_RECURSE ANT_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ANT_HEADER_FILES "./include/ant/*.h" "./include/ant/*.hpp")

create_source_groups(src)
create_source_groups("include/ant")

add_library(ant ${ANT_SOURCE_FILES} ${ANT_HEADER_FILES})


# tests

file(GLOB_RECURSE TEST_SOURCE_FILES "./test/*.cpp")
file(GLOB GTEST_SOURCES "./test/gtest/*")
file(GLOB_RECURSE GTEST_HEADER_FILES "./include/gtest/*.h")

create_source_groups(test)
create_source_groups("include/gtest")
add_executable(test ${TEST_SOURCE_FILES} ${GTEST_SOURCES} ${GTEST_HEADER_FILES})

target_link_libraries(test ant)


# app/bst

file(GLOB BST_FILES "./app/bst/*")
add_executable(bst ${BST_FILES}) 

# app/segment_intersections

file(GLOB SEG_INTERSECTS "./app/segment_intersections/*")
add_executable(segment_intersections ${SEG_INTERSECTS})








