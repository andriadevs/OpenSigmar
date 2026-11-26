cmake_minimum_required(VERSION 3.16)
project(OpenSigmar)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (RUNTIME_OUTPUT_SUBDIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${RUNTIME_OUTPUT_SUBDIRECTORY}")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# libSigmar
if (${LIBSIGMAR})
    # Search headers and libraries in the target environment
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

    add_library(Sigmar STATIC src/Sigmar.c) # Build libSigmar for linking with platform implementations
    #set_target_properties(Sigmar PROPERTIES
    #    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
    target_include_directories(Sigmar PRIVATE include)
endif() # LIBSIGMAR

# SDL3
if (${SDL3})
    #if (${LINUX})
        #set(SDL_STATIC ON)
        add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL) # Build SDL3 submodule
    #endif() # LINUX

    # Build OpenSigmar SDL implementation
    add_executable(OpenSigmarSDL src/sdl/main.c src/sdl/Platform_SDL.c)
    set_target_properties(OpenSigmarSDL PROPERTIES
        OUTPUT_NAME "OpenSigmar")

    if (${WINDOWS})
        add_compile_options(-mwindows)
        add_custom_command( # Copy SDL3.dll from lib to bin
            TARGET OpenSigmarSDL POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/lib/SDL3.dll $<TARGET_FILE_DIR:OpenSigmarSDL>)
    endif() # WINDOWS

    target_include_directories(OpenSigmarSDL PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(OpenSigmarSDL PRIVATE Sigmar SDL3::SDL3)
    target_compile_definitions(OpenSigmarSDL PUBLIC SIGMAR_SDL)

    # Copy assets directory to bin
    add_custom_command(
        TARGET OpenSigmarSDL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/sdl $<TARGET_FILE_DIR:OpenSigmarSDL>/assets)
endif() # SDL3

# TI-84 Plus CE
if (${TI84PCE})
    add_custom_target(SigmarTI84PCE ALL # Build OpenSigmar TI-84 Plus CE implementation
        make -f ${CMAKE_SOURCE_DIR}/src/ti84pce/Makefile BINDIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif() # TI84PCE