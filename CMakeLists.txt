cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

set(HAL_BUILD_TESTS TRUE)
set(HAL_BUILD_EXAMPLES TRUE)

add_subdirectory(halcyon)

message("Include dirs = ${HALCYON_INCLUDE_DIRS}")

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

list(TRANSFORM QUEST_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(HalodaQuest
${QUEST_SOURCES}
)

if (NOT MSVC)
    target_compile_options(HalodaQuest PRIVATE -Wall -Wextra)
else()
    target_compile_options(HalodaQuest PRIVATE /W4)
endif()

target_include_directories(HalodaQuest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(HalodaQuest PRIVATE Halcyon)

set_target_properties(HalodaQuest PROPERTIES
CXX_STANDARD 23
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)