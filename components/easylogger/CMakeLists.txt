if (CONFIG_ENABLE_EASYLOGGER)
    FetchContent_Declare(
        easylogger
        URL https://github.com/armink/EasyLogger/archive/refs/tags/2.2.0.tar.gz
    )

    FetchContent_MakeAvailable(easylogger)
endif()