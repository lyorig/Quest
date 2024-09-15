cmake_minimum_required(VERSION 3.22.0)

project(Quest
	VERSION 0.1
	LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Quest's build type.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)

add_subdirectory(halcyon)

add_executable(TheGame WIN32
	src/scene/console.cpp
	src/scene/dummy.cpp
	src/scene/main_menu.cpp
	src/scene/manager.cpp
	src/atlas.cpp
	src/game.cpp
	src/helpers.cpp
	src/main.cpp
	src/sprite.cpp
	src/field.cpp
)

target_compile_features(TheGame PRIVATE
	cxx_std_23
)

# Is there a strip utility?
if (CMAKE_STRIP)
	add_custom_command(TARGET TheGame POST_BUILD
		COMMAND 
		"$<$<NOT:$<CONFIG:Debug,RelWithDebInfo>>:${CMAKE_STRIP};$<TARGET_FILE:TheGame>>"
		COMMAND_EXPAND_LISTS VERBATIM
	)
endif()

# Warnings.
if (MSVC)
	set(WarningFlags /Wall)
else()
	set(WarningFlags -Wall -Wpedantic)
endif()

add_compile_options(${WarningFlags})

# Link-time optimization.
if (MSVC)
	set(LtoCompileFlags /GL)
	set(LtoLinkFlags /LTCG)
else()
	set(LtoCompileFlags -flto)
	set(LtoLinkFlags -flto)
endif()

target_compile_options(TheGame PRIVATE
	$<$<CONFIG:Release>:${LtoCompileFlags}>
	$<$<NOT:$<OR:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>>:-march=native>
)

target_link_options(TheGame PRIVATE
	$<$<CONFIG:Release>:${LtoLinkFlags}>
)

target_include_directories(TheGame PRIVATE
	include/
)

target_link_libraries(TheGame PRIVATE
	Halcyon::Halcyon
)

set_target_properties(TheGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
	$<NOT:$<CONFIG:Debug>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

install(FILES $<TARGET_RUNTIME_DLLS:TheGame> TYPE BIN)
