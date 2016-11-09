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

cmake_minimum_required (VERSION 3.2.2)
project ( Ant )

# this is used by DEBUG and RELEASE
list( APPEND CMAKE_CXX_FLAGS "-std=c++1y -Werror -Wno-error=char-subscripts -Wno-char-subscripts" )

set( CODECOVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
set( CODECOVERAGE_LINKER_FLAGS "--coverage" )


# to use with your favourite IDE
if( CMAKE_CONFIGURATION_TYPES )
    list( APPEND CMAKE_CONFIGURATION_TYPES CodeCoverage )
endif()

# to use with makefile and configuration for IDE
set( CMAKE_CXX_FLAGS_CODECOVERAGE "${CODECOVERAGE_COMPILE_FLAGS}" )
# don't know if I need to add CMAKE_EXE_LINKER_FLAGS
set( CMAKE_EXE_LINKER_FLAGS_CODECOVERAGE "${CMAKE_EXE_LINKER_FLAGS} ${CODECOVERAGE_LINKER_FLAGS}" )


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

find_library(GTEST_LIBRARY gtest)
if (NOT GTEST_LIBRARY)
  message(FATAL_ERROR "gtest library not found")
endif()


include_directories("./include")
include_directories("/usr/local/include")
include_directories("/usr/local/lib")


# ant library

file(GLOB_RECURSE ANT_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ANT_HEADER_FILES "./include/ant/*.h" "./include/ant/*.hpp")

create_source_groups(src)
create_source_groups("include/ant")

add_library(ant ${ANT_SOURCE_FILES} ${ANT_HEADER_FILES})


# tests

file(GLOB_RECURSE TEST_SOURCE_FILES "./test/*.cpp")

create_source_groups(test)

add_executable(ant_test ${TEST_SOURCE_FILES})
target_link_libraries(ant_test ant)
target_link_libraries(ant_test gtest)


# app/bst

file(GLOB BST_FILES "./app/bst/*")
add_executable(bst ${BST_FILES})


# app/segment_intersections

# for now we won't build this target until you fix it
# file(GLOB SEG_INTERSECTS "./app/segment_intersections/*")
# add_executable(segment_intersections ${SEG_INTERSECTS})

file(GLOB TRIANGULATION_FILES "./app/triangulation/*")
add_executable(triangulation ${TRIANGULATION_FILES})
target_link_libraries(triangulation ant)


# benchmarks

file(GLOB BENCHMARKS_FILES "./benchmarks/*.cpp")

add_executable(benchmarks ${BENCHMARKS_FILES})
target_link_libraries(benchmarks ant)
target_link_libraries(benchmarks gtest)
