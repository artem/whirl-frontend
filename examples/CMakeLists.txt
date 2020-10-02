cmake_minimum_required(VERSION 3.5)

message(STATUS "Examples")

# All tests target

add_executable(whirl_example main.cpp)
target_link_libraries(whirl_example whirl)
