cmake_minimum_required(VERSION 3.14)
project(MOVRPDD)
set (CMAKE_CXX_STANDARD 17)
set(INPUT_DIR "${PROJECT_SOURCE_DIR}/input/")
set(OUTPUT_DIR "${PROJECT_SOURCE_DIR}/output/")

find_package(Python COMPONENTS Interpreter REQUIRED)

option(CPU_TIME  "Enable cputime build" ON)

configure_file(${PROJECT_SOURCE_DIR}/movrpdd/config.h.in ${PROJECT_SOURCE_DIR}/movrpdd/include/config.h) 
include_directories(${PROJECT_SOURCE_DIR}/movrpdd/include)

enable_testing()
add_subdirectory(movrpdd)
add_subdirectory(tests)

add_test(
    NAME inst_pequena
    COMMAND $<TARGET_FILE:inst_pequena>
    )
