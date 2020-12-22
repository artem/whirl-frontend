cmake_minimum_required(VERSION 3.14)

include(FetchContent)

# Offline mode
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# --------------------------------------------------------------------

FetchContent_Declare(
        await
        GIT_REPOSITORY https://gitlab.com/Lipovsky/await.git
        GIT_TAG master
)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(await)

# --------------------------------------------------------------------

FetchContent_Declare(
        wheels
        GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
        GIT_TAG master
)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(wheels)

# --------------------------------------------------------------------

FetchContent_Declare(
        cereal
        GIT_REPOSITORY https://github.com/USCiLab/cereal.git
        GIT_TAG master
)

set(FETCHCONTENT_QUIET OFF)

# FetchContent_MakeAvailable(cereal)
FetchContent_Populate("cereal")
set(cereal_INCLUDE_DIR "${cereal_SOURCE_DIR}/include" PARENT_SCOPE)

# --------------------------------------------------------------------

FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(fmtlib)

# --------------------------------------------------------------------

FetchContent_Declare(
        ctti
        GIT_REPOSITORY https://github.com/Manu343726/ctti
        GIT_TAG master
)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(ctti)
