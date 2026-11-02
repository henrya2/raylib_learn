# add_example_game(<name> [SOURCES <file>...] [ASSETS_DIR <dir>] [WORKING_DIR <dir>])
#
# Turns one folder under examples/ into a playable target:
#
#   * executable <name>, linked against raylib
#   * sources default to everything under src/ (auto re-globbed by CMake)
#   * assets/ (or ASSETS_DIR) is copied next to the executable on every build
#   * the game starts in its own folder, so relative asset paths work both
#     from the build directory and from the IDE
#   * grouped under the "examples" folder in Visual Studio / Xcode
#
# Typical examples/CMakeLists.txt for a game is just:
#
#   add_example_game(minimal_game ASSETS_DIR assets)

function(add_example_game GAME_NAME)
    cmake_parse_arguments(GAME "" "ASSETS_DIR;WORKING_DIR" "SOURCES" ${ARGN})

    if(GAME_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_example_game(${GAME_NAME}): unknown arguments: ${GAME_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT GAME_SOURCES)
        file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        )
    endif()

    if(NOT GAME_SOURCES)
        message(FATAL_ERROR "add_example_game(${GAME_NAME}): no sources found under ${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    add_executable(${GAME_NAME} ${GAME_SOURCES})

    target_link_libraries(${GAME_NAME} PRIVATE raylib)
    target_include_directories(${GAME_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    if(NOT GAME_WORKING_DIR)
        if(GAME_ASSETS_DIR)
            set(GAME_WORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${GAME_ASSETS_DIR}")
        else()
            set(GAME_WORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif()
    endif()

    set_target_properties(${GAME_NAME} PROPERTIES
        FOLDER "examples"
        VS_DEBUGGER_WORKING_DIRECTORY "${GAME_WORKING_DIR}"
    )

    # Assets travel with the executable, so a game can just open "assets/x.png".
    if(GAME_ASSETS_DIR)
        set(_assets_src "${CMAKE_CURRENT_SOURCE_DIR}/${GAME_ASSETS_DIR}")
        if(IS_DIRECTORY "${_assets_src}")
            add_custom_command(TARGET ${GAME_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                        "${_assets_src}" "$<TARGET_FILE_DIR:${GAME_NAME}>/${GAME_ASSETS_DIR}"
                VERBATIM
            )
        else()
            message(STATUS "${GAME_NAME}: no '${GAME_ASSETS_DIR}' folder yet, skipping asset copy")
        endif()
    endif()

    if(MSVC)
        target_compile_options(${GAME_NAME} PRIVATE /W4)
        target_compile_definitions(${GAME_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${GAME_NAME} PRIVATE -Wall -Wextra)
    endif()
endfunction()
