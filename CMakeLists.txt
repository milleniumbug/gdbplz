cmake_minimum_required(VERSION 3.3)
project(gdbplz)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

find_package(Boost 1.54 COMPONENTS system filesystem REQUIRED)
find_package(Threads)

set(CMAKE_CXX_EXTENSIONS OFF)
file(GLOB_RECURSE INCLUDES include/*.cpp)
file(GLOB SOURCES src/*.cpp)

set(SOURCE_FILES
	${INCLUDES}
	${SOURCES}
	../../NotMine/jucipp/tiny-process-library/process.hpp
	../../NotMine/jucipp/tiny-process-library/process.cpp
	../../NotMine/jucipp/tiny-process-library/process_unix.cpp)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories(../../NotMine/jucipp/tiny-process-library)

add_library(gdbplz ${SOURCE_FILES})
set_property(TARGET gdbplz PROPERTY CXX_STANDARD 11)
target_link_libraries(gdbplz Threads::Threads ${Boost_LIBRARIES})

file(GLOB EXAMPLE_FILES "examples/*.cpp")
foreach(sample ${EXAMPLE_FILES})
	get_filename_component(sample_we ${sample} NAME_WE)
	add_executable(${sample_we} ${sample})
	set_property(TARGET ${sample_we} PROPERTY CXX_STANDARD 11)
	target_link_libraries(${sample_we} gdbplz Threads::Threads ${Boost_LIBRARIES})
endforeach()

file(GLOB TEST_FILES "tests/*.cpp")
add_executable(gdbplz_tests ${TEST_FILES})
set_property(TARGET gdbplz_tests PROPERTY CXX_STANDARD 11)
target_link_libraries(gdbplz_tests gdbplz Threads::Threads ${Boost_LIBRARIES})