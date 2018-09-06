cmake_minimum_required (VERSION 3.10)
project (simple-json C CXX)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall")
endif()

add_library(myjson myjson.c cpp/myjson.cpp cpp/myjson.h cpp/test.cpp cpp/myjson_base.h)
add_executable(simple-json test.c)
add_executable(simple-json-cpp cpp/test.cpp)
target_link_libraries(simple-json myjson)
