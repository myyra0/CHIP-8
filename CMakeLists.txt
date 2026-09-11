cmake_minimum_required(VERSION 4.3)
set(CHIP_8_VERSION "0.1.0" CACHE STRING "CHIP-8 release version")
project(CHIP_8 VERSION ${CHIP_8_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(CHIP_8 src/main.c
        src/stack.c
        src/stack.h
        src/main.h
        src/instructions.c
        src/instructions.h
        src/timer.c
        src/timer.h
        src/display.c
        src/display.h
        src/audio.c
        src/audio.h)


find_package(SDL3 REQUIRED)
target_link_libraries(CHIP_8 PRIVATE SDL3::SDL3)

if(UNIX AND NOT APPLE)
        set_target_properties(CHIP_8 PROPERTIES INSTALL_RPATH "$ORIGIN")
elseif(APPLE)
        set_target_properties(CHIP_8 PROPERTIES INSTALL_RPATH "@loader_path")
endif()

target_compile_options(CHIP_8 PRIVATE
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:-Wall;-Wextra;-Wpedantic>
)

include(GNUInstallDirs)

install(TARGETS CHIP_8 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY roms/ DESTINATION ${CMAKE_INSTALL_DATADIR}/CHIP_8/roms
        FILES_MATCHING PATTERN "*.ch8")

if(TARGET SDL3::SDL3-shared)
    install(IMPORTED_RUNTIME_ARTIFACTS SDL3::SDL3-shared
            DESTINATION ${CMAKE_INSTALL_BINDIR})
elseif(TARGET SDL3::SDL3)
    install(IMPORTED_RUNTIME_ARTIFACTS SDL3::SDL3
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

set(CPACK_PACKAGE_NAME "CHIP_8")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHIP-8 emulator")
set(CPACK_PACKAGE_FILE_NAME "CHIP_8-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR "ZIP;TGZ" CACHE STRING "CPack generators to use")
include(CPack)
