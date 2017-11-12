project(chip8 VERSION 0.0.1 LANGUAGES CXX)

# set (CMAKE_CXX_STANDARD 11)

file(GLOB chip8_SOURCES "src/*.c" "src/*.cpp")

add_library(chip8 ${chip8_SOURCES})

add_definitions(-Dchip8_VERSION="${chip8_VERSION}")

target_include_directories(chip8 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)

target_link_libraries(chip8 glfw)

install(TARGETS chip8 EXPORT chip8Config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)  # This is for Windows

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT chip8Config DESTINATION share/chip8/cmake)

export(TARGETS chip8 FILE chip8Config.cmake)