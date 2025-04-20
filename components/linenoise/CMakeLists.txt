include(FetchContent)

FetchContent_Declare(
    linenoise
    URL https://github.com/antirez/linenoise/archive/refs/tags/1.0.tar.gz
)

FetchContent_MakeAvailable(linenoise)
