cmake_minimum_required(VERSION 3.22.0)

project(Quest
	VERSION 0.1
	LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Quest's build type.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)

add_subdirectory(halcyon)

add_executable(game WIN32
	src/scene/console.cpp
	src/scene/dummy.cpp
	src/scene/main_menu.cpp
	src/scene/manager.cpp
	src/game.cpp
	src/helpers.cpp
	src/main.cpp
	src/sprite.cpp
	src/field.cpp
)

if (WIN32)
	target_compile_definitions(Halcyon PRIVATE
		$<$<CONFIG:Debug>:HAL_DEBUG_ADVANCED>
	)

	target_compile_definitions(game PRIVATE
		$<$<CONFIG:Debug>:HAL_DEBUG_ADVANCED>
	)
endif()

target_compile_features(game PRIVATE
	cxx_std_23
)

# Is there a strip utility?
if (CMAKE_STRIP)
	add_custom_command(TARGET game POST_BUILD
		COMMAND 
		"$<$<NOT:$<CONFIG:Debug,RelWithDebInfo>>:${CMAKE_STRIP};$<TARGET_FILE:game>>"
		COMMAND_EXPAND_LISTS VERBATIM
	)
endif()

target_compile_options(game PRIVATE
	$<$<NOT:$<OR:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>>:-march=native> # Optimize to the max!
	$<IF:$<CXX_COMPILER_ID:MSVC>,/Wall,-Wall -Wpedantic> # Enable warnings.
)

target_include_directories(game PRIVATE
	include/
)

target_link_libraries(game PRIVATE
	Halcyon::Halcyon
)

# Enable LTO.
set_target_properties(game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
	$<NOT:$<CONFIG:Debug>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

install(FILES $<TARGET_RUNTIME_DLLS:game> TYPE BIN)
