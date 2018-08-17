cmake_minimum_required (VERSION 3.10)
project (myjson_test C CXX)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(myjson myjson.cpp)
add_executable(myjson_test test.cpp)
target_link_libraries(myjson_test myjson)
