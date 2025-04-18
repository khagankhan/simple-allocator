cmake_minimum_required(VERSION 3.10)
project(SimpleAllocator C)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIR})
set(SOURCES
    ${SOURCE_DIR}/mm.c
    ${SOURCE_DIR}/memlib.c
)

option(SHARED "Build shared libraries as well..." OFF)
if(SHARED) 
    message(STATUS "Building as shared library as well...")
    add_library(allocator_shared SHARED ${SOURCES})
endif() 

option(DEBUG "Enabled DEBUG mode..." OFF)
if(DEBUG)
    add_compile_definitions(DEBUG)
    message(STATUS "Debug mode enabled...")
endif()

    message(STATUS "Building executable...")
    add_executable(allocator ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/main.c)