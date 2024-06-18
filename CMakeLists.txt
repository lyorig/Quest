cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

include(halcyon/config.cmake)

set(QUEST_SOURCES
scenes/main_menu.cpp
console.cpp
game.cpp
helpers.cpp
main.cpp
scene_manager.cpp
sprite.cpp
field.cpp
)

list(TRANSFORM QUEST_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(HalodaQuest
${HALCYON_SOURCES}
${QUEST_SOURCES}
)

if (NOT MSVC)
    target_compile_options(HalodaQuest PRIVATE -Wall -Wextra)
else()
    target_compile_options(HalodaQuest PRIVATE /W4)
endif()

target_include_directories(HalodaQuest PRIVATE ${HALCYON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(HalodaQuest PRIVATE ${HALCYON_LIBRARIES})

set_target_properties(HalodaQuest PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)