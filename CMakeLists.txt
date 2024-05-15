cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

include(halcyon/config.cmake)

set(QUEST_SOURCES
state/main_menu.cpp
console.cpp
game.cpp
helpers.cpp
main.cpp
sprite.cpp
text_field.cpp
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

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(HalodaQuest PRIVATE HAL_DEBUG_ENABLED)
endif()

target_include_directories(HalodaQuest PRIVATE ${HALCYON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(HalodaQuest PRIVATE ${HALCYON_LIBRARIES})

set_target_properties(HalodaQuest PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)