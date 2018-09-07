cmake_minimum_required (VERSION 3.10)
project (simple-json C CXX)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall")
endif()

set(google_test_dir ./googletest-master)
#Add the google test subdirectory
add_subdirectory(${google_test_dir})
#include googletest/include dir
include_directories(${google_test_dir}/googletest/include)
#include the googlemock/include dir
include_directories(${google_test_dir}/googlemock/include)

# Simple-json C version
add_library(myjson myjson.c)
add_executable(simple-json test.c)
target_link_libraries(simple-json myjson)

# Simple-json C++ version
add_library(myjson-cpp cpp/myjson.cpp cpp/myjson.h cpp/test.cpp cpp/myjson_base.h)
add_executable(simple-json-cpp cpp/test.cpp)
target_link_libraries(simple-json-cpp myjson-cpp)

# Link with GoogleTest
target_link_libraries(simple-json-cpp gtest gtest_main)
# Link with GoogleMock
target_link_libraries(simple-json-cpp gmock gmock_main)
