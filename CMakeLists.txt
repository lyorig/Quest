cmake_minimum_required(VERSION 3.22.0)

project(Quest VERSION 0.1 LANGUAGES CXX)

add_subdirectory(halcyon)

set(QUEST_SOURCES
    scene/console.cpp
    scene/main_menu.cpp
    scene/manager.cpp
	atlas.cpp
    game.cpp
    helpers.cpp
    main.cpp
    sprite.cpp
    field.cpp
)

list(TRANSFORM QUEST_SOURCES PREPEND src/)

add_executable(TheGame WIN32
    ${QUEST_SOURCES}
)

# Warnings.
if (MSVC)
	set(WarningFlags /Wall)
else()
	set(WarningFlags -Wall -Wpedantic)
endif()

target_compile_options(TheGame PRIVATE ${WarningFlags})

# Link-time optimization.
if (CMAKE_BUILD_TYPE MATCHES Release)
	if (MSVC)
		set(LtoCompileFlags /GL)
		set(LtoLinkFlags /LTCG)
	else()
		set(LtoCompileFlags -flto)
		set(LtoLinkFlags -flto)
	endif()

	target_compile_options(TheGame PRIVATE ${LtoCompileFlags})
	target_link_options(TheGame PRIVATE ${LtoLinkFlags})
endif()

target_include_directories(TheGame PRIVATE include/)
target_link_libraries(TheGame PRIVATE Halcyon::Halcyon)

set_target_properties(TheGame PROPERTIES
    CXX_STANDARD 23
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)

add_custom_command(TARGET TheGame POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:TheGame> $<TARGET_FILE_DIR:TheGame>
  COMMAND_EXPAND_LISTS
  )
