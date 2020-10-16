cmake_minimum_required(VERSION 3.5)

message(STATUS "Hello example")

add_executable(whirl_example_hello main.cpp)
target_link_libraries(whirl_example_hello whirl-v1)
