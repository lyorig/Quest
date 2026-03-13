cmake_minimum_required(VERSION 3.30.0)

project(Quest
	VERSION 0.1
	LANGUAGES CXX
)

# Halcyon has nifty helper functions that we'd like to use as well.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/halcyon/cmake)
include(HalcyonHelpers)

set(QUEST_SOURCES
	scene/console
	scene/dummy
	scene/main_menu
	scene/manager
	atlas
	data_loader
	field
	game
	helpers
	main
	sprite
)

list(TRANSFORM QUEST_SOURCES PREPEND src/)
list(TRANSFORM QUEST_SOURCES APPEND .cpp)

set(QUEST_HEADERS
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
	data_loader
	entity_manager
	field
	game
	helpers
	rfem
	sprite
	static_vector
	types
)

list(TRANSFORM QUEST_HEADERS PREPEND include/quest/)
list(TRANSFORM QUEST_HEADERS APPEND .hpp)

add_executable(game WIN32)
target_sources(game
	PRIVATE ${QUEST_SOURCES}
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES ${QUEST_HEADERS}
)

target_compile_options(game PRIVATE
	$<IF:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>,/Wall,-Wall -Wpedantic -Wextra -Wno-c++98-compat>
)

set_std_no_ext(game 23)

add_subdirectory(halcyon)
add_subdirectory(rectpack2D)

target_link_libraries(game PUBLIC
	Halcyon::Halcyon
	rectpack2D::rectpack2D
)

# Enable LTO in release modes.
set_target_properties(game PROPERTIES
	$<NOT:$<CONFIG:Debug>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

# On Windows, ensure the system can find the necessary DLLs.
install(FILES $<TARGET_RUNTIME_DLLS:game> TYPE BIN)

install(TARGETS game DESTINATION bin)
install(DIRECTORY data DESTINATION .)
