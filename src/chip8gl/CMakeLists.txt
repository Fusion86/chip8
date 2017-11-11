project(chip8gl VERSION 0.0.1 LANGUAGES CXX)

include_directories(${OPENGL_INCLUDE_DIR} include)

file(GLOB chip8gl_SOURCES "src/*.c" "src/*.cpp")

add_executable(chip8gl ${chip8gl_SOURCES})

target_link_libraries(chip8gl chip8 glfw)

add_definitions(-Dchip8gl_VERSION="${chip8gl_VERSION}")