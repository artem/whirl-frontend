cmake_minimum_required(VERSION 3.14)
project(whirl)

include(cmake/CheckCompiler.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -g -fno-omit-frame-pointer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

option(WHIRL_DEVELOPER "Whirl development mode" OFF)

add_subdirectory(third_party)

add_subdirectory(whirl)
#add_subdirectory(tests)

add_subdirectory(examples)
