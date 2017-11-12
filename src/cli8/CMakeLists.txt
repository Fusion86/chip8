project(cli8 VERSION 0.0.1 LANGUAGES CXX)

include_directories(include)

file(GLOB cli8_SOURCES "src/*.c" "src/*.cpp")

add_executable(cli8 ${cli8_SOURCES})

target_link_libraries(cli8 chip8)

add_definitions(-Dcli8_VERSION="${cli8_VERSION}")
