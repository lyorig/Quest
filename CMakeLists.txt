cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

include(halcyon/config.cmake)

add_executable(HalodaQuest main.cpp ${HALCYON_SOURCES})

target_compile_options(HalodaQuest PRIVATE -flto -march=native)
target_link_options(HalodaQuest PRIVATE -flto)

target_compile_definitions(HalodaQuest PRIVATE HAL_DEBUG_ENABLED)

target_include_directories(HalodaQuest PRIVATE ${HALCYON_INCLUDE_DIRS})
target_link_libraries(HalodaQuest PRIVATE ${HALCYON_LIBRARIES})