cmake_minimum_required (VERSION 2.6)
project (tinyjson_test C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(tinyjson tinyjson.c)
add_executable(tinyjson_test test.c)
target_link_libraries(tinyjson_test tinyjson)
