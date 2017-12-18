project(lucina8 VERSION 0.0.1 LANGUAGES CXX)

find_package(CTRULIB REQUIRED)

include_directories(include)

file(GLOB lucina8_SOURCES "src/*.c" "src/*.cpp")

add_executable(lucina8 ${lucina8_SOURCES})

target_link_libraries(lucina8 chip8 3ds::ctrulib)

add_definitions(-Dlucina8_VERSION="${lucina8_VERSION}")

add_3dsx_target(lucina8)

add_custom_command(
    TARGET lucina8_3dsx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lucina8.* ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
