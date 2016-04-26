cmake_minimum_required(VERSION 3.3)
project(gdbplz)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")
endif()

find_package(Boost 1.54 COMPONENTS system filesystem log REQUIRED)
find_package(Threads)
add_subdirectory(rapidcheck)
add_subdirectory(yet_another_process_library)

set(CMAKE_CXX_EXTENSIONS OFF)
file(GLOB_RECURSE INCLUDES "include/*.hpp")
file(GLOB SOURCES "src/*.cpp")

set(SOURCE_FILES
	${INCLUDES}
	${SOURCES})
	
add_library(gdbplz ${SOURCE_FILES})
target_include_directories(gdbplz
PUBLIC
	include
	wiertlo/include
PRIVATE
	src/include)
set_property(TARGET gdbplz PROPERTY CXX_STANDARD 11)
target_link_libraries(gdbplz Threads::Threads ${Boost_LIBRARIES} yet_another_process_library)
target_compile_definitions(gdbplz PRIVATE -DBOOST_LOG_DYN_LINK)

file(GLOB EXAMPLE_FILES "examples/*.cpp")
foreach(sample ${EXAMPLE_FILES})
	get_filename_component(sample_we ${sample} NAME_WE)
	add_executable(${sample_we} ${sample})
	set_property(TARGET ${sample_we} PROPERTY CXX_STANDARD 14)
	target_link_libraries(${sample_we} gdbplz Threads::Threads ${Boost_LIBRARIES} rapidcheck)
endforeach()

file(GLOB TEST_DATA_FILES "tests/test_data/*.cpp")
foreach(sample ${TEST_DATA_FILES})
	get_filename_component(sample_we ${sample} NAME_WE)
	add_executable(${sample_we} ${sample})
	set_property(TARGET ${sample_we} PROPERTY CXX_STANDARD 14)
	target_compile_options(${sample_we} PRIVATE -g)
	target_link_libraries(${sample_we} gdbplz Threads::Threads ${Boost_LIBRARIES} rapidcheck)
endforeach()

file(GLOB TEST_FILES "tests/*.cpp")
add_executable(gdbplz_tests ${TEST_FILES})
set_property(TARGET gdbplz_tests PROPERTY CXX_STANDARD 14)
target_link_libraries(gdbplz_tests gdbplz Threads::Threads ${Boost_LIBRARIES} rapidcheck)
target_include_directories(gdbplz_tests
PRIVATE
	src/include)