if (CONFIG_ENABLE_CM_BACKTRACE)
    FetchContent_Declare(
        cmbacktrace
        URL https://github.com/armink/CmBacktrace/archive/refs/tags/1.4.1.tar.gz
    )

    FetchContent_MakeAvailable(cmbacktrace)
endif()