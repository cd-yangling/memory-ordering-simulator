cmake_minimum_required(VERSION 3.17)

project(memory-ordering-simulator-asm)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(parser mos_asm.y
	${CMAKE_CURRENT_BINARY_DIR}/parser.c
	DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
)

FLEX_TARGET(scanner mos_asm.l
	${CMAKE_CURRENT_BINARY_DIR}/scanner.c
	DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h
)

add_executable(mos_asm
	mos_asm.c
	${BISON_parser_OUTPUTS}
	${FLEX_scanner_OUTPUTS}
)

target_include_directories(mos_asm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

