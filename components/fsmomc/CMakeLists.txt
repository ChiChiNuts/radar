if (CONFIG_ENABLE_LINENOISE)
    FetchContent_Declare(
        fsmomc
        GIT_REPOSITORY https://github.com/Wormarz/fsmomc.git
        GIT_TAG dev-0.1
    )

    FetchContent_MakeAvailable(fsmomc)
endif()