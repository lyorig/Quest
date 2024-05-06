cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

include(halcyon/config.cmake)

file(GLOB_RECURSE QUEST_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(HalodaQuest WIN32
${HALCYON_SOURCES}
${QUEST_SOURCES}
)

# target_compile_options(HalodaQuest PRIVATE -flto -march=native)
# target_link_options(HalodaQuest PRIVATE -flto)

target_compile_options(HalodaQuest PRIVATE -Wall -Wextra)

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(HalodaQuest PRIVATE -flto -march=native)
    target_link_options(HalodaQuest PRIVATE -flto)

    target_compile_definitions(HalodaQuest PRIVATE HAL_FAST_TYPES HAL_DEBUG_ENABLED)
endif()

target_include_directories(HalodaQuest PRIVATE ${HALCYON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(HalodaQuest PRIVATE ${HALCYON_LIBRARIES})

target_compile_definitions(HalodaQuest PRIVATE HAL_INTEGRAL_COORD)

set_target_properties(HalodaQuest PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
RUNTIME_OUTPUT_NAME $<TARGET_NAME:HalodaQuest>-${CMAKE_BUILD_TYPE})