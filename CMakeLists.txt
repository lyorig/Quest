cmake_minimum_required(VERSION 3.28...4.4)

project(Quest VERSION 0.1 LANGUAGES CXX)

# Halcyon has nifty helper functions that we'd like to use as well.
cmake_path(APPEND halcyon_modules ${CMAKE_CURRENT_SOURCE_DIR} halcyon cmake)
list(APPEND CMAKE_MODULE_PATH ${halcyon_modules})

include(HalcyonHelpers)

halcyon_setupbuildtype(Release)

halcyon_createsourcelist(sources src cpp
    scene/console
    scene/dummy
    scene/main_menu
    scene/manager
    game
    main
)

halcyon_createsourcelist(headers include/quest hpp
    scene/base
    scene/console
    scene/dummy
    scene/main_menu
    scene/manager
    util/move_only_function
    animatable
    constants
    entity_manager
    game
    rfem
    static_vector
    types
)

halcyon_createsourcelist(modules src cpp
    atlas
    commands/defs
    data_loader
    field
    helpers
    sprite
)

add_executable(game WIN32)
target_sources(
    game
    PRIVATE
        FILE_SET SOURCES BASE_DIRS src FILES ${sources}
        FILE_SET CXX_MODULES BASE_DIRS src FILES ${modules}
    PUBLIC
        FILE_SET HEADERS BASE_DIRS include FILES ${headers}
)

set(cxx_frontend ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT})

if(cxx_frontend STREQUAL "MSVC")
    target_compile_options(game PRIVATE /Wall)
elseif(cxx_frontend STREQUAL "GNU")
    target_compile_options(game PRIVATE -Wall -Wpedantic)
endif()

halcyon_setstandardwithoutextensions(game 23)

add_subdirectory(halcyon)
add_subdirectory(rectpack2D)

target_link_libraries(game PUBLIC Halcyon::Halcyon rectpack2D::rectpack2D)

include(CheckIPOSupported)
check_ipo_supported(RESULT has_ipo LANGUAGES CXX)

if(has_ipo)
    set_target_properties(
        game
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION ON
            INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
    )
endif()

# On Windows, ensure the system can find the necessary DLLs.
install(FILES $<TARGET_RUNTIME_DLLS:game> TYPE BIN)

install(TARGETS game DESTINATION bin)
install(DIRECTORY data DESTINATION .)

# Put compile commands somewhere clangd etc. can see them.
if(CMAKE_EXPORT_COMPILE_COMMANDS AND PROJECT_IS_TOP_LEVEL)
    cmake_path(APPEND link_from ${CMAKE_BINARY_DIR} compile_commands.json)
    cmake_path(
        APPEND link_to
        ${CMAKE_SOURCE_DIR}
        build
        compile_commands.json
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${link_from}" "${link_to}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
