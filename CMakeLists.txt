#
# test is actually depends on google test
#
# -DCMAKE_BUILD_TYPE=Release - to compile in release mode
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


set( CMAKE_CONFIGURATION_TYPES  "Debug;Release;Coverage;Safe" CACHE STRING "" FORCE )


# this is used by DEBUG and RELEASE
#-fconcepts - works only for gcc and should not be used
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -march=native -Werror -Wno-error=char-subscripts -Wno-char-subscripts" )

# Release
set( CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG" )

# Safe
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_SAFE " ${CMAKE_CXX_FLAGS_DEBUG} -D_LIBCPP_DEBUG=1")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_SAFE " ${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

endif()

# Coverage
set( CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage" )
set( CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS} --coverage" )

find_library(RCU_LIBS NAMES urcu urcu-qsbr urcu-cds)


link_directories("/usr/local/lib")

include_directories("./include")
include_directories("/usr/local/include")

add_subdirectory(./submodules/googletest EXCLUDE_FROM_ALL)

# ant library

file(GLOB_RECURSE ANT_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ANT_HEADER_FILES "./include/ant/*.h" "./include/ant/*.hpp")

create_source_groups(src)
create_source_groups("include/ant")

add_library(ant ${ANT_SOURCE_FILES} ${ANT_HEADER_FILES})

install(TARGETS ant ARCHIVE DESTINATION /usr/local/lib)
install(DIRECTORY include/ant DESTINATION /usr/local/include)

# tests

file(GLOB_RECURSE TEST_SOURCE_FILES "./test/*.cpp")
file(GLOB_RECURSE RCU_TEST_SOURCE_FILES "./test/parallel/rcu/*.cpp")
foreach (path ${RCU_TEST_SOURCE_FILES})
    list (REMOVE_ITEM TEST_SOURCE_FILES ${path})
endforeach()

create_source_groups(test)

add_executable(ant_test ${TEST_SOURCE_FILES})
target_link_libraries(ant_test ant gtest pthread)

if (RCU_LIBS)
    add_executable(ant_rcu_test ${RCU_TEST_SOURCE_FILES} "./test/main.cpp")
    target_compile_definitions(ant_rcu_test PRIVATE DEBUG_RCU)
    target_link_libraries(ant_rcu_test ant gtest_one_thread pthread urcu urcu-qsbr urcu-cds)
endif()

# app/bst

file(GLOB BST_FILES "./app/bst/*")
add_executable(bst ${BST_FILES})


# app/segment_intersections

# TODO for now we won't build this target until you fix it
# file(GLOB SEG_INTERSECTS "./app/segment_intersections/*")
# add_executable(segment_intersections ${SEG_INTERSECTS})

file(GLOB TRIANGULATION_FILES "./app/triangulation/*")
add_executable(triangulation ${TRIANGULATION_FILES})
target_link_libraries(triangulation ant)

# benchmarks

if (CMAKE_BUILD_TYPE MATCHES Release)

    file(GLOB_RECURSE BENCHMARKS_FILES "./benchmarks/*.cpp")
    file(GLOB_RECURSE RCU_BENCHMARKS_FILES "./benchmarks/parallel/rcu/*.cpp")
    foreach (path ${RCU_BENCHMARKS_FILES})
        list (REMOVE_ITEM BENCHMARKS_FILES ${path})
    endforeach()

    add_executable(benchmarks ${BENCHMARKS_FILES})
    target_link_libraries(benchmarks ant gtest benchmark pthread)

    if (RCU_LIBS)
        add_executable(rcu_benchmarks ${RCU_BENCHMARKS_FILES} "./benchmarks/main.cpp")
        target_link_libraries(rcu_benchmarks ant benchmark gtest pthread urcu urcu-qsbr urcu-cds)
    endif()

endif()