# Compile options

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-O3 -Wall -Wextra -Wpedantic -g -fno-omit-frame-pointer)
# add_compile_options(-Werror)
