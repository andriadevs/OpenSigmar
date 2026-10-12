cmake_minimum_required(VERSION 3.16)
project(OpenSigmar)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(OpenSigmar
    src/main.c
    src/Platform_SDL.c
    src/Sigmar.c
)

target_compile_definitions(OpenSigmar PUBLIC SIGMAR_SDL)

target_link_libraries(OpenSigmar PRIVATE SDL3::SDL3)

add_custom_command(
    TARGET OpenSigmar POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE_DIR:OpenSigmar>/OpenSigmar
            ${CMAKE_SOURCE_DIR}/bin)