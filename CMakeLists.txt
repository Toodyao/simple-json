cmake_minimum_required (VERSION 3.10)
project (myjson_test C CXX)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall")
endif()

add_library(myjson myjson.c)
add_executable(myjson_test test.c)
target_link_libraries(myjson_test myjson)
