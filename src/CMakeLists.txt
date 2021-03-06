project(conan-example)
cmake_minimum_required(VERSION 3.5..3.16)


### enable if troubleshooting cmake find_package() stuff
#set(CMAKE_FIND_DEBUG_MODE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

### toolchain file generated by conan
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# ### debug dump
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

#FILE(GLOB  FREERTOS_PLUS_TRACE_SOURCES  ${FREERTOS_PLUS_DIR}/Source/FreeRTOS-Plus-Trace/*.c)
list(APPEND FREERTOS_SOURCES
    # project-local bits
    "freertos/console.c"
    "freertos/freertos.c"
    "freertos/run-time-stats-utils.c"

    # dynamic memory stuff (malloc(), free())
    "${FREERTOS_DIR}/portable/MemMang/heap_3.c"

    # posix port
    "${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c"
    "${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix/port.c"

    # freertos kernel
    "${FREERTOS_DIR}/croutine.c"
    "${FREERTOS_DIR}/event_groups.c"
    "${FREERTOS_DIR}/list.c"
    "${FREERTOS_DIR}/queue.c"
    "${FREERTOS_DIR}/stream_buffer.c"
    "${FREERTOS_DIR}/tasks.c"
    "${FREERTOS_DIR}/timers.c"
)

# Debug
message("=== freertos cmake file ===================================")
message("Project: ${PROJECT_NAME}")
message("Sources: ${FREERTOS_SOURCES}")
message("Sources: ${FREERTOS_PLUS_TRACE_SOURCES}")
message("")
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}")
message("")
message("FREERTOS_DIR = ${FREERTOS_DIR}")               # provided by conanfile
message("FREERTOS_PLUS_DIR = ${FREERTOS_PLUS_DIR}")     # provided by conanfile
message("===========================================================")


add_definitions(-DTRACE_ENABLED=0)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3 -O0")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -ggdb3 -O0")


### include libraries
find_package(libnlohmann-json CONFIG)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(${CMAKE_PROJECT_NAME}
        ${FREERTOS_SOURCES}
#        ${FREERTOS_PLUS_TRACE_SOURCES}
        main.cpp
        platform/linux.cpp
    )
else()  ### else,  bare metal build
    add_executable(${CMAKE_PROJECT_NAME}
        ${FREERTOS_SOURCES}
        main.cpp
        platform/baremetal.cpp
    )
endif()

#include_directories(${CMAKE_INCLUDE_PATH})
target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../include/freertos
    ${FREERTOS_DIR}/include
    ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix
    ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix/utils
    ${FREERTOS_DIR}/Demo/Common/include
#    ${FREERTOS_PLUS_DIR}/Source/FreeRTOS-Plus-Trace/Include
    ${CMAKE_INCLUDE_PATH}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    libnlohmann-json::libnlohmann-json
    pthread
)
