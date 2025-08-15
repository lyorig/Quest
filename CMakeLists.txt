cmake_minimum_required(VERSION 3.22.0)

project(Quest
	VERSION 0.1
	LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Quest's build type.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)

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
)

list(TRANSFORM SOURCES PREPEND src/)
list(TRANSFORM SOURCES APPEND .cpp)

add_executable(game WIN32
	${SOURCES}
)

if (WIN32)
	target_compile_definitions(Halcyon PRIVATE
		$<$<CONFIG:Debug>:HAL_DEBUG_ADVANCED>
	)

	target_compile_definitions(game PRIVATE
		$<$<CONFIG:Debug>:HAL_DEBUG_ADVANCED>
	)
endif()

# Is there a strip utility?
if (CMAKE_STRIP)
	add_custom_command(TARGET game POST_BUILD
		COMMAND 
		"$<$<NOT:$<CONFIG:Debug,RelWithDebInfo>>:${CMAKE_STRIP};$<TARGET_FILE:game>>"
		COMMAND_EXPAND_LISTS VERBATIM
	)
endif()

# Enable warnings.
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

set(BinPath ${CMAKE_CURRENT_SOURCE_DIR})
cmake_path(APPEND BinPath bin)

# Enable LTO.
set_target_properties(game PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BinPath}
	$<NOT:$<CONFIG:Debug>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

install(FILES $<TARGET_RUNTIME_DLLS:game> TYPE BIN)
