# add_game(<name> [SOURCES <file>...])
#
# Turns one folder under games/ into a playable target:
#
#   * executable <name> in <repo>/bin/<platform>/<config>/, linked against raylib
#   * sources default to everything under src/ (auto re-globbed by CMake):
#     *.c / *.cpp are compiled, *.h / *.hh / *.hpp / *.hxx / *.inl are attached
#     to the target as headers - not compiled, but visible in the IDE
#   * headers live next to their .cpp in src/ and are included with the same
#     relative path: #include "player.h"  (src/ is on the include path)
#   * NO asset copying: at startup the game changes into its own assets folder
#     (<repo>/assets/<name>/) and loads assets with bare filenames. The path
#     from the .exe folder to that folder is fixed and handed to the sources as:
#
#         GAME_ASSETS_DIR   "../../../assets/<name>/"
#
#     Typical main.cpp:
#         ChangeDirectory(GetApplicationDirectory());
#         ChangeDirectory(GAME_ASSETS_DIR);
#         Sound coin = LoadSound("coin.wav");
#
#   * the debugger starts the game in its .exe directory, and the game anchors
#     to it via GetApplicationDirectory(), so paths resolve the same way from
#     the IDE, from Explorer and from a shell
#
# A game's CMakeLists.txt is therefore just:
#
#   add_game(minimal_game)

function(add_game GAME_NAME)
    cmake_parse_arguments(GAME "" "" "SOURCES" ${ARGN})

    if(GAME_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "add_game(${GAME_NAME}): unknown arguments: ${GAME_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT GAME_SOURCES)
        file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        )
    endif()

    # Headers are not compiled, but adding them to the target makes them show up
    # in the IDE (VS "Header Files", CMake Tools / CLion project tree) and makes
    # CONFIGURE_DEPENDS re-run cmake when one is added or removed.
    file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
    )

    if(NOT GAME_SOURCES)
        message(FATAL_ERROR "add_game(${GAME_NAME}): no sources found under ${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    add_executable(${GAME_NAME} ${GAME_SOURCES} ${GAME_HEADERS})

    if(GAME_HEADERS)
        # Belt and braces: never hand a header to a compiler, whatever its suffix.
        set_source_files_properties(${GAME_HEADERS} PROPERTIES HEADER_FILE_ONLY ON)

        # Keep the on-disk layout in the IDE instead of one flat folder.
        source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${GAME_HEADERS})
    endif()

    target_link_libraries(${GAME_NAME} PRIVATE raylib)
    target_include_directories(${GAME_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    # Fixed relative path from the .exe folder to <repo>/assets/<name>/ - no
    # copying. The game cd's into it at startup and loads bare filenames.
    target_compile_definitions(${GAME_NAME} PRIVATE
        GAME_ASSETS_DIR="${RAYLIB_LEARN_ASSETS_FROM_EXE}${GAME_NAME}/"
    )

    # NOTE: do NOT set VS_DEBUGGER_WORKING_DIRECTORY here. CMake's file-api hands
    # that value to CMake Tools verbatim, so a generator expression such as
    # "$<TARGET_FILE_DIR:...>" ends up as a literal cwd and the debug launch fails
    # with "Invalid 'cwd' value ... syntax is incorrect". Both CMake Tools launch
    # and debug already default to the .exe directory, which is what we want.
    set_target_properties(${GAME_NAME} PROPERTIES
        FOLDER "games"
    )

    if(MSVC)
        target_compile_options(${GAME_NAME} PRIVATE /W4)
        target_compile_definitions(${GAME_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${GAME_NAME} PRIVATE -Wall -Wextra)
    endif()
endfunction()
