cmake_minimum_required(VERSION 3.30.0)

project(Quest
	VERSION 0.1
	LANGUAGES CXX
)

# Halcyon has nifty helper functions that we'd like to use as well.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/halcyon/cmake)
include(HalcyonHelpers)

Halcyon_SetupBuildType(Release)

Halcyon_CreateSourceList(QUEST_SOURCES src cpp
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

Halcyon_CreateSourceList(QUEST_HEADERS include/quest hpp
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

add_executable(game WIN32)
target_sources(game
	PRIVATE ${QUEST_SOURCES}
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES ${QUEST_HEADERS}
)

target_compile_options(game PRIVATE
	$<IF:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>,/Wall,-Wall -Wpedantic -Wextra>
)

Halcyon_SetStandardWithoutExtensions(game 23)

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
