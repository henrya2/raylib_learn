cmake_minimum_required(VERSION 3.22)

project(raylib_learn
    VERSION 0.1.0
    DESCRIPTION "Learning raylib - a collection of small games"
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Global build settings
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# ---------------------------------------------------------------------------
# Output layout - executables go to the REPO ROOT, not into the build tree:
#
#   <repo>/bin/<platform>/<config>/<game>.exe      e.g. bin/win64/debug/
#   <build>/lib/<platform>/<config>/raylib.lib     intermediate artifacts
#
# The build tree only holds intermediate files. Nothing is copied: the games
# load their assets straight from <repo>/assets/ through the fixed relative
# path from the .exe (see RAYLIB_LEARN_ASSETS_FROM_EXE below).
# ---------------------------------------------------------------------------
if(WIN32)
    set(_os "win")
elseif(APPLE)
    set(_os "macos")
else()
    set(_os "linux")
endif()

if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
    set(RAYLIB_LEARN_PLATFORM "${_os}-arm64")     # e.g. macos-arm64
else()
    math(EXPR _bits "${CMAKE_SIZEOF_VOID_P} * 8")
    set(RAYLIB_LEARN_PLATFORM "${_os}${_bits}")   # e.g. win64 / linux64
endif()
unset(_os)

message(STATUS "Output platform dir: ${RAYLIB_LEARN_PLATFORM}")

# $<LOWER_CASE:$<CONFIG>> -> "debug" / "release" / "relwithdebinfo" / "minsizerel".
# The generator expression also stops multi-config generators from appending
# yet another per-configuration sub-directory.
set(_config_layout "${RAYLIB_LEARN_PLATFORM}/$<LOWER_CASE:$<CONFIG>>")

# Executables (and raylib.dll when built shared) go to <repo>/bin/...
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${_config_layout}")

# Static/shared libs are intermediate artifacts: keep them in the build tree.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${_config_layout}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${_config_layout}")

# ---------------------------------------------------------------------------
# Assets - one folder per game under <repo>/assets/, never copied:
#   assets/<game>/      used by that game only (no shared assets)
#
# The .exe sits in <repo>/bin/<platform>/<config>/, i.e. exactly three levels
# below the repo root, so the path from the .exe back to the assets folder is
# fixed. Each game changes into its own assets/<game>/ folder at startup (see
# add_game) and then loads assets with bare filenames.
# ---------------------------------------------------------------------------
set(RAYLIB_LEARN_ASSETS_FROM_EXE "../../../assets/")
message(STATUS "Assets path from the executable: ${RAYLIB_LEARN_ASSETS_FROM_EXE}")

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(RAYLIB_LEARN_BUILD_GAMES "Build the games under games/" ON)
option(RAYLIB_LEARN_SHARED_RAYLIB "Link the games against raylib as a shared library" OFF)

# Leave empty to build every game. Set it to e.g. "minimal_game;my_game" to
# build only a subset: cmake -S . -B build -DRAYLIB_LEARN_GAMES=minimal_game
set(RAYLIB_LEARN_GAMES "" CACHE STRING "Semicolon separated list of games to build (empty = all)")

# ---------------------------------------------------------------------------
# raylib (git submodule, tag 6.2)
# ---------------------------------------------------------------------------
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/raylib/CMakeLists.txt")
    message(FATAL_ERROR
        "thirdparty/raylib is empty. Fetch the submodule first:\n"
        "    git submodule update --init --recursive")
endif()

# raylib is consumed as a sub-project: its own 300+ examples are never built,
# we only want the library target.
set(BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ${RAYLIB_LEARN_SHARED_RAYLIB} CACHE BOOL "" FORCE)
set(CUSTOMIZE_BUILD  OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/raylib)

if(TARGET raylib)
    set_target_properties(raylib PROPERTIES FOLDER "thirdparty/raylib")
endif()

# ---------------------------------------------------------------------------
# Games
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddGame)

if(RAYLIB_LEARN_BUILD_GAMES)
    add_subdirectory(games)
endif()
