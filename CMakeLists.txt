cmake_minimum_required(VERSION 3.22.0)

project(HalodaQuest)

include(halcyon/config.cmake)

file(GLOB_RECURSE QUEST_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(HalodaQuest
${HALCYON_SOURCES}
${QUEST_SOURCES}
)

# target_compile_options(HalodaQuest PRIVATE -flto -march=native)
# target_link_options(HalodaQuest PRIVATE -flto)

if (NOT MSVC)
    target_compile_options(HalodaQuest PRIVATE -Wall -Wextra)
endif()


if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    if (NOT MSVC)
        target_compile_options(HalodaQuest PRIVATE -flto -march=native)
        target_link_options(HalodaQuest PRIVATE -flto)
    endif()

    target_compile_definitions(HalodaQuest PRIVATE HAL_DEBUG_ENABLED)
endif()

target_include_directories(HalodaQuest PRIVATE ${HALCYON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(HalodaQuest PRIVATE ${HALCYON_LIBRARIES})

target_compile_definitions(HalodaQuest PRIVATE HAL_INTEGRAL_COORD)

set_target_properties(HalodaQuest PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)