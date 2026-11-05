# Every sub-directory of games/ that owns a CMakeLists.txt is a game.
#
# To add a new game:
#   1. run `python scripts/new_game.py <my_game>` (creates the folder, sources
#      and assets for you), or manually create games/<my_game>/CMakeLists.txt
#      with a single add_game(<my_game>) call and games/<my_game>/src/main.cpp
#   2. re-run cmake
# Nothing here has to be edited. Prefix a folder with "_" to skip it.

file(GLOB _candidates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")

set(_games "")
foreach(_dir IN LISTS _candidates)
    # Skip files and anything explicitly opted out ("_template", ".cache", ...)
    if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_dir}")
        continue()
    endif()
    if(_dir MATCHES "^[_.]")
        continue()
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_dir}/CMakeLists.txt")
        message(STATUS "Skipping '${_dir}': no CMakeLists.txt (not a game yet?)")
        continue()
    endif()
    # Optional whitelist: -DRAYLIB_LEARN_GAMES=minimal_game;my_game
    if(RAYLIB_LEARN_GAMES AND NOT _dir IN_LIST RAYLIB_LEARN_GAMES)
        continue()
    endif()

    list(APPEND _games "${_dir}")
endforeach()

list(SORT _games)

foreach(_game IN LISTS _games)
    message(STATUS "Game: ${_game}")
    add_subdirectory("${_game}")
endforeach()

if(NOT _games)
    message(STATUS "No games found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()
