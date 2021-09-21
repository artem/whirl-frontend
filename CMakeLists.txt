cmake_minimum_required(VERSION 3.14)
project(whirl-frontend)

include(cmake/CheckCompiler.cmake)

option(WHIRL_DEVELOPER "Whirl development mode" OFF)
option(WHIRL_EXAMPLES "Enable Whirl examples" OFF)

include(cmake/CompileOptions.cmake)

add_subdirectory(third_party)

add_subdirectory(whirl)
#add_subdirectory(tests)

if(WHIRL_DEVELOPER OR WHIRL_EXAMPLES)
    add_subdirectory(examples)
endif()
