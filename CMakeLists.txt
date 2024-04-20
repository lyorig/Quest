cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

include(halcyon/config.cmake)

set(QUEST_SOURCES
state/main_menu.cpp
state/window_expansion.cpp
app.cpp
arguments.cpp
game.cpp
main.cpp
)

list(TRANSFORM QUEST_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(HalodaQuest
${HALCYON_SOURCES}
${QUEST_SOURCES}
)

# target_compile_options(HalodaQuest PRIVATE -flto -march=native)
# target_link_options(HalodaQuest PRIVATE -flto)

target_compile_options(HalodaQuest PRIVATE -Wall -Wextra)

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(HalodaQuest PRIVATE -flto -march=native)
    target_link_options(HalodaQuest PRIVATE -flto)

    target_compile_definitions(HalodaQuest PRIVATE HAL_FAST_TYPES)
endif()

target_include_directories(HalodaQuest PRIVATE ${HALCYON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(HalodaQuest PRIVATE ${HALCYON_LIBRARIES})

target_compile_definitions(HalodaQuest PRIVATE HAL_INTEGRAL_COORD)

set_target_properties(HalodaQuest PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
RUNTIME_OUTPUT_NAME $<TARGET_NAME:HalodaQuest>-${CMAKE_BUILD_TYPE})