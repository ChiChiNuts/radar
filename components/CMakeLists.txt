include(FetchContent)

set(SUBDIRECTORIES)

file(GLOB ALL_ITEMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(ITEM ${ALL_ITEMS})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ITEM})
        list(APPEND SUBDIRECTORIES ${ITEM})
    endif()
endforeach()

message(STATUS "Found subdirectories: ${SUBDIRECTORIES}")

foreach(SUBDIR ${SUBDIRECTORIES})
    add_subdirectory(${SUBDIR})
endforeach()
