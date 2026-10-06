cmake_minimum_required(VERSION 3.28...4.3)

project(Quest VERSION 0.1 LANGUAGES CXX)

# Halcyon has nifty helper functions that we'd like to use as well.
cmake_path(APPEND MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "halcyon" "cmake")
list(APPEND CMAKE_MODULE_PATH ${MODULE_PATH})
include(HalcyonHelpers)

halcyon_setupbuildtype(Release)

halcyon_createsourcelist(QUEST_SOURCES src cpp
    scene/console
    scene/dummy
    scene/main_menu
    scene/manager
    atlas
    game
    main
)

halcyon_createsourcelist(QUEST_HEADERS include/quest hpp
    commands/defs
    scene/base
    scene/console
    scene/dummy
    scene/main_menu
    scene/manager
    util/move_only_function
    animatable
    atlas
    constants
    entity_manager
    game
    rfem
    static_vector
    types
)

add_executable(game WIN32)
target_sources(
    game
    PRIVATE
        FILE_SET SOURCES #
            BASE_DIRS src
            FILES ${QUEST_SOURCES}
        FILE_SET CXX_MODULES #
            BASE_DIRS src
            FILES
                src/data_loader.cpp
                src/field.cpp
                src/helpers.cpp
                src/sprite.cpp
    PUBLIC
        FILE_SET HEADERS #
            BASE_DIRS include
            FILES ${QUEST_HEADERS}
)

if(MSVC)
    target_compile_options(game PRIVATE /Wall)
else()
    target_compile_options(game PRIVATE -Wall -Wpedantic -Wextra)
endif()

halcyon_setstandardwithoutextensions(game 23)

add_subdirectory(halcyon)
add_subdirectory(rectpack2D)

target_link_libraries(game PUBLIC Halcyon::Halcyon rectpack2D::rectpack2D)

include(CheckIPOSupported)
check_ipo_supported(RESULT HAS_IPO LANGUAGES CXX)

if(HAS_IPO)
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
    cmake_path(APPEND LINK_FROM "${CMAKE_BINARY_DIR}" "compile_commands.json")
    cmake_path(
        APPEND LINK_TO
        "${CMAKE_SOURCE_DIR}"
        "build"
        "compile_commands.json"
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${LINK_FROM}" "${LINK_TO}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
