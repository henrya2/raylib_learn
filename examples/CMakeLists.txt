# Every sub-directory of examples/ that owns a CMakeLists.txt is a game.
#
# To add a new game:
#   1. create examples/<my_game>/src/main.cpp
#   2. create examples/<my_game>/CMakeLists.txt with a single
#      add_example_game(<my_game>) call
#   3. re-run cmake
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
    # Optional whitelist: -DRAYLIB_LEARN_EXAMPLES=minimal_game;pong
    if(RAYLIB_LEARN_EXAMPLES AND NOT _dir IN_LIST RAYLIB_LEARN_EXAMPLES)
        continue()
    endif()

    list(APPEND _games "${_dir}")
endforeach()

list(SORT _games)

foreach(_game IN LISTS _games)
    message(STATUS "Example game: ${_game}")
    add_subdirectory("${_game}")
endforeach()

if(NOT _games)
    message(STATUS "No example games found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()
