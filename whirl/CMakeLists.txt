cmake_minimum_required(VERSION 3.5)

# --------------------------------------------------------------------

get_filename_component(LIB_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(LIB_PATH "." ABSOLUTE)

file(GLOB_RECURSE LIB_CXX_SOURCES ${LIB_PATH}/*.cpp)
file(GLOB_RECURSE LIB_HEADERS ${LIB_PATH}/*.hpp)

add_library(whirl STATIC ${LIB_CXX_SOURCES} ${LIB_HEADERS})
target_include_directories(whirl PUBLIC ${LIB_INCLUDE_PATH})

# --------------------------------------------------------------------

# Dependencies

target_link_libraries(whirl PUBLIC wheels await fmt ctti)

# Be silent, cereal
target_include_directories(whirl PUBLIC ${cereal_INCLUDE_DIR})

# --------------------------------------------------------------------

# Workaround for std::filesystem

if(WHIRL_NO_FS)
    message(STATUS "No std::filesystem")
    target_compile_definitions(whirl PUBLIC WHIRL_NO_FS=1)
endif()

# Force logging

if(WHIRL_FORCE_LOGGING)
    message(STATUS "Force logging")
    target_compile_definitions(whirl PUBLIC WHIRL_FORCE_LOGGING=1)
endif()

# --------------------------------------------------------------------

# Linters

if(WHIRL_DEVELOPER)

if(CLANG_FORMAT_TOOL)
    add_clang_format_target(
            whirl_clang_format
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${LIB_HEADERS} ${LIB_CXX_SOURCES})
endif()

if(CLANG_TIDY_TOOL)
    add_clang_tidy_target(
            whirl_clang_tidy
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${LIB_INCLUDE_PATH}
            ${LIB_HEADERS} ${LIB_CXX_SOURCES})
endif()

endif()
