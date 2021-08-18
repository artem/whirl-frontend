message(STATUS "HelloWorld example")

add_executable(whirl_example_hello main.cpp)
target_link_libraries(whirl_example_hello whirl)
