cmake_minimum_required(VERSION 3.0)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
# include(ExternalProject)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include(GNUInstallDirs)

execute_process(
    COMMAND git describe --dirty --always --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE COMMITHASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions(-DCOMMITHASH="${COMMITHASH}")

if(3DS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DKA_SUGGESTED_C_FLAGS}")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/../cmake)
    include(Tools3DS)

    add_subdirectory(chip8)
    add_subdirectory(lucina8)
else()
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(glfw)
    add_subdirectory(chip8)
    add_subdirectory(cli8)
    add_subdirectory(chip8gl)
endif()
