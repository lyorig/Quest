cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest VERSION 0.1 LANGUAGES CXX)

set(Halcyon_BUILD_TESTS TRUE)
set(Halcyon_BUILD_EXAMPLES TRUE)

add_subdirectory(halcyon)

set(QUEST_SOURCES
    scene/console.cpp
    scene/main_menu.cpp
    scene/manager.cpp
    game.cpp
    helpers.cpp
    main.cpp
    sprite.cpp
    field.cpp
)

list(TRANSFORM QUEST_SOURCES PREPEND src/)

add_executable(HalodaQuest
    ${QUEST_SOURCES}
)

if(NOT MSVC)
    target_compile_options(HalodaQuest PRIVATE -Wall -Wextra)
else()
    target_compile_options(HalodaQuest PRIVATE /W4)
endif()

target_include_directories(HalodaQuest PRIVATE include/)
target_link_libraries(HalodaQuest PRIVATE Halcyon::Halcyon)

set_target_properties(HalodaQuest PROPERTIES
    CXX_STANDARD 23
    RUNTIME_OUTPUT_DIRECTORY bin/
)