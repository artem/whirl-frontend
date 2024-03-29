include(FetchContent)

# --------------------------------------------------------------------

# Offline mode
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)
# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

message(STATUS "FetchContent: wheels")

FetchContent_Declare(
        wheels
        GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
        GIT_TAG master
)
FetchContent_MakeAvailable(wheels)

# --------------------------------------------------------------------

message(STATUS "FetchContent: await")

FetchContent_Declare(
        await
        GIT_REPOSITORY https://gitlab.com/Lipovsky/await.git
        GIT_TAG master
)
FetchContent_MakeAvailable(await)

# --------------------------------------------------------------------

message(STATUS "FetchContent: timber")

FetchContent_Declare(
        timber
        GIT_REPOSITORY https://gitlab.com/whirl-framework/timber.git
        GIT_TAG master
)
FetchContent_MakeAvailable(timber)

# --------------------------------------------------------------------

message(STATUS "FetchContent: muesli")

FetchContent_Declare(
        muesli
        GIT_REPOSITORY https://github.com/artem/muesli.git
        GIT_TAG master
)
FetchContent_MakeAvailable(muesli)

# --------------------------------------------------------------------

message(STATUS "FetchContent: commute")

FetchContent_Declare(
        commute
        GIT_REPOSITORY https://gitlab.com/whirl-framework/commute.git
        GIT_TAG master
)
FetchContent_MakeAvailable(commute)

# --------------------------------------------------------------------

message(STATUS "FetchContent: persist")

FetchContent_Declare(
        persist
        GIT_REPOSITORY https://gitlab.com/whirl-framework/persist.git
        GIT_TAG master
)
FetchContent_MakeAvailable(persist)

# --------------------------------------------------------------------

message(STATUS "FetchContent: fmt")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fmt)
