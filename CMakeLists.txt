cmake_minimum_required(VERSION 3.13)

project(memory-ordering-simulator)

add_compile_options(-Wall -Wextra -Werror -g)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++20")

option(ENABLE_RELEASE "enable release" ON)
if (ENABLE_RELEASE STREQUAL ON)
	add_compile_options(-O2)
endif()

option(ENABLE_FAIR_BUS_ARBITRATION "enable fair bus arbitration" OFF)
if (ENABLE_FAIR_BUS_ARBITRATION STREQUAL ON)
	add_definitions(-D HAVE_FAIR_BUS_ARBITRATION)
endif()

option(ENABLE_STORE_BUFFER "enable_store_buffer" ON)
if (ENABLE_STORE_BUFFER STREQUAL ON)
	add_definitions(-D HAVE_STORE_BUFFER)
endif()

option(ENABLE_INVALID_QUEUE "enable invalid queue" ON)
if (ENABLE_INVALID_QUEUE STREQUAL ON)
	add_definitions(-D HAVE_INVALID_QUEUE)
endif()

option (ENABLE_BSB_TEST "enable back side bus test" OFF)
if (ENABLE_BSB_TEST STREQUAL ON)
	add_definitions(-D HAVE_BSB_TEST)
endif()

option (ENABLE_FSB_TEST "enable front side bus test" OFF)
if (ENABLE_FSB_TEST STREQUAL ON)
	add_definitions(-D HAVE_FSB_TEST)
endif()

set(LIST_SOURCES
	basic_type.cpp
	cache_line.cpp
	cache_set.cpp
	cache_ccfsm.cpp
	bsb.cpp
	fsb.cpp
	cache.cpp
	memory.cpp
	processor.cpp
	test/bsb_test.cpp
	test/fsb_test.cpp
	main.cpp
)

add_subdirectory(assembler)

add_executable(mos_run ${LIST_SOURCES})

