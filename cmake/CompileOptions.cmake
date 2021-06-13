# Compile options

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -g -fno-omit-frame-pointer)

if(WHIRL_DEVELOPER)
    message(STATUS "Turn warnings into errors")
    add_compile_options(-Werror)
endif()
