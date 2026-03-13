cmake_minimum_required(VERSION 3.30.0)

project(Quest
	VERSION 0.1
	LANGUAGES CXX
)

set(QUEST_SOURCES
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

target_compile_features(game PUBLIC cxx_std_23)

add_subdirectory(halcyon)
add_subdirectory(rectpack2D)

target_link_libraries(game PUBLIC
	Halcyon::Halcyon
	rectpack2D::rectpack2D
)

set(QUEST_BIN_PATH ${CMAKE_SOURCE_DIR})
cmake_path(APPEND QUEST_BIN_PATH bin)

# Enable LTO in release modes.
set_target_properties(game PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${QUEST_BIN_PATH}
	$<NOT:$<CONFIG:Debug>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

# On Windows, ensure the system can find the necessary DLLs.
install(FILES $<TARGET_RUNTIME_DLLS:game> TYPE BIN)
