cmake_minimum_required(VERSION 3.5)

message(STATUS "KV example")

add_executable(whirl_example_kv main.cpp)
target_link_libraries(whirl_example_kv whirl-v1)
