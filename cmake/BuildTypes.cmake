message(STATUS "Adding build types...")

set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -fno-sanitize-recover=all"
    CACHE STRING "Flags used by the C++ compiler during Asan builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_ASAN "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined"
    CACHE STRING "Flags used for linking binaries during Asan builds."
    FORCE)

mark_as_advanced(CMAKE_CXX_FLAGS_ASAN CMAKE_EXE_LINKER_FLAGS_ASAN)
