cmake_minimum_required(VERSION 3.22.0)

project(Quest
 VERSION 0.1
 LANGUAGES CXX
)

add_subdirectory(halcyon)
add_subdirectory(rectpack2D)

set(SOURCES
 scene/console
 scene/dummy
 scene/main_menu
 scene/manager
 atlas
 field
 game
 helpers
 main
 sprite
 thread_pool
)

list(TRANSFORM SOURCES PREPEND src/)
list(TRANSFORM SOURCES APPEND .cpp)

add_executable(game WIN32
 ${SOURCES}
)

target_compile_features(game PRIVATE cxx_std_23)

target_compile_options(game PRIVATE
 $<IF:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>,/Wall,-Wall -Wpedantic -Wextra -Wno-c++98-compat>
)

target_include_directories(game PRIVATE
 include
)

target_link_libraries(game PRIVATE
 Halcyon::Halcyon
 rectpack2D::rectpack2D
)

include(ProcessorCount)
ProcessorCount(CPU_COUNT)
target_compile_definitions(game PRIVATE HQ_CPU_COUNT=${CPU_COUNT})

set(BinPath ${CMAKE_SOURCE_DIR})
cmake_path(APPEND BinPath bin)

# Enable LTO.
set_target_properties(game PROPERTIES
 RUNTIME_OUTPUT_DIRECTORY ${BinPath}
 $<NOT:$<CONFIG:Debug>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

install(FILES $<TARGET_RUNTIME_DLLS:game> TYPE BIN)
