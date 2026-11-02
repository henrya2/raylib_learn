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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Every game (and raylib.dll when built shared) ends up in <build>/bin.
# The $<1:...> generator expression suppresses the extra per-configuration
# sub-directory multi-config generators would otherwise append, so the
# executables always sit next to their assets and next to raylib.dll.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/bin>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/lib>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/lib>")

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(RAYLIB_LEARN_BUILD_EXAMPLES "Build the games under examples/" ON)
option(RAYLIB_LEARN_SHARED_RAYLIB "Link the games against raylib as a shared library" OFF)

# Leave empty to build every example. Set it to e.g. "minimal_game;pong" to
# build only a subset: cmake -S . -B build -DRAYLIB_LEARN_EXAMPLES=minimal_game
set(RAYLIB_LEARN_EXAMPLES "" CACHE STRING "Semicolon separated list of example games to build (empty = all)")

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
include(AddExampleGame)

if(RAYLIB_LEARN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
