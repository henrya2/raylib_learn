# raylib_learn

> **Current state:** an early-stage learning project — a CMake scaffold for building
> small [raylib](https://github.com/raysan5/raylib) games, with one fully
> implemented reference game. Version `0.1.0`.

## At a glance

| Item | Value |
| --- | --- |
| Purpose | "Learning raylib - a collection of small games" (from `CMakeLists.txt`) |
| Languages | C99, C++20 (`CMAKE_CXX_EXTENSIONS OFF`) |
| Build system | CMake >= 3.22 |
| Dependency | raylib 6.2 as a git submodule (`thirdparty/raylib`, pinned at `0256645`, tag `6.2`) |
| Repository | `https://github.com/henrya2/raylib_learn.git`, branch `main` |
| Git state | 2 commits, clean working tree; latest `b78c652` "clean up" (2026-11-05) |
| Platform | Developed on Windows x64; CMake handles win / macos / linux |

## Current state

- **One game exists:** `minimal_game` ("Gem Catcher") — see below. It is the
  reference skeleton that every other game is meant to copy.
- The build has been configured and run locally: `build/`,
  `cmake-build-debug/`, `cmake-build-release/` and
  `cmake-build-relwithdebinfo/` all contain a `CMakeCache.txt`, and
  `bin/win64/debug/` and `bin/win64/relwithdebinfo/` contain built
  `minimal_game.exe` files (all ignored by git).
- There are **no tests, CI, presets, license file, or prior documentation** —
  this README is the first Markdown file in the repository.
- No `CMakePresets.json`; configuration is done with plain `cmake -S/-B`.

## Repository layout

```
raylib_learn/
├── CMakeLists.txt              # top-level: settings, output layout, raylib, games
├── cmake/
│   └── AddGame.cmake           # add_game() helper — defines every game target
├── games/
│   ├── CMakeLists.txt          # auto-discovers game sub-directories
│   └── minimal_game/
│       ├── CMakeLists.txt      # single line: add_game(minimal_game)
│       └── src/main.cpp        # "Gem Catcher"
├── assets/
│   └── minimal_game/           # per-game assets, loaded by bare filename
│       ├── coin.wav
│       └── timeup.wav
├── scripts/
│   └── new_game.py             # scaffolds a new game (Python 3, stdlib only)
├── thirdparty/
│   └── raylib/                 # git submodule, tag 6.2
├── bin/                        # generated executables (ignored)
└── build/, cmake-build-*       # generated build trees (ignored)
```

## Getting started

raylib is a submodule, so a plain clone leaves `thirdparty/raylib` empty and
CMake fails with an explicit `FATAL_ERROR` telling you to fetch it.

```sh
git clone --recurse-submodules https://github.com/henrya2/raylib_learn.git
cd raylib_learn

# If you already cloned without submodules:
git submodule update --init --recursive

# Configure and build (multi-config generators such as Visual Studio / CLion
# need --config; single-config generators can omit it)
cmake -S . -B build
cmake --build build --config Debug
```

The executable lands at `bin/win64/debug/minimal_game.exe` on this machine
(`bin/<platform>/<config>/<game>.exe` in general). Platform names: `win64`,
`linux64`, `macos64` or `macos-arm64`.

### Build options

| Option | Default | Meaning |
| --- | --- | --- |
| `RAYLIB_LEARN_BUILD_GAMES` | `ON` | Build the targets under `games/` |
| `RAYLIB_LEARN_SHARED_RAYLIB` | `OFF` | Link raylib as a shared library instead of static |
| `RAYLIB_LEARN_GAMES` | `""` | Semicolon-separated whitelist, e.g. `-DRAYLIB_LEARN_GAMES=minimal_game`; empty builds all |

raylib is added as a sub-project with `BUILD_EXAMPLES=OFF` and
`CUSTOMIZE_BUILD=OFF`, so its 300+ examples are never compiled.

## How the build is wired

`add_game(<name>)` in `cmake/AddGame.cmake` does all the work:

- Globs `src/*.c` / `src/*.cpp` as sources and `src/*.h*` as headers, both with
  `CONFIGURE_DEPENDS` so adding a file re-runs CMake automatically.
- Headers are marked `HEADER_FILE_ONLY` and grouped by their on-disk path so
  they appear in the IDE project tree without being compiled.
- Links `raylib`, puts `src/` on the include path, and sets `/W4` on MSVC or
  `-Wall -Wextra` elsewhere.
- Defines `GAME_ASSETS_DIR` as the fixed relative path from the `.exe` to that
  game's assets folder.
- Places the target in the IDE folder `games`.

**Asset convention (no copying).** Executables go to
`bin/<platform>/<config>/` — exactly three levels below the repo root — so the
path back to assets is fixed: `../../../assets/<game>/`. At startup each game
anchors to its own `.exe` directory via `GetApplicationDirectory()`, then does
`ChangeDirectory(GAME_ASSETS_DIR)` and loads assets with bare filenames such as
`LoadSound("coin.wav")`. This makes paths resolve identically from the IDE,
Explorer, or a shell. `AddGame.cmake` deliberately does **not** set
`VS_DEBUGGER_WORKING_DIRECTORY`, because CMake file-api passes that value
verbatim and a generator expression there breaks CMake Tools launches.

## Games

### `minimal_game` — "Gem Catcher"

960x540 window at 60 FPS. Move the blue circle with **WASD / arrow keys** to
collect randomly spawned gems; each gem is worth 10 points. A 30-second timer
ends the round — press **R** to restart, **Esc** or the window close button to
quit. `coin.wav` plays on pickup, `timeup.wav` when the round ends. Audio is
opened with `InitAudioDevice()`, which is safe even if no device is available.

## Adding a game

```sh
python scripts/new_game.py my_game
cmake -S . -B build          # re-run CMake to pick up the new target
```

`new_game.py` creates `games/<name>/CMakeLists.txt`,
`games/<name>/src/main.cpp` (a minimal raylib window) and
`assets/<name>/.gitkeep`. Names must start with a letter and contain only
letters, digits and underscores; the script refuses to overwrite an existing
game or assets folder.

`games/CMakeLists.txt` globs sub-directories automatically, so nothing needs
editing there. Any directory without a `CMakeLists.txt` is skipped, and
directories starting with `_` or `.` are ignored (useful for templates).
Alternatively, create the folder manually with a `CMakeLists.txt` containing
just `add_game(<name>)` and a `src/main.cpp`, then re-run CMake.

## Ignored / generated files

`.gitignore` excludes `/bin/`, `build/`, `build-*/`, `out/`, `cmake-build-*/`,
IDE directories (`.vs/`, `.vscode/`, `.idea/`, `*.user`) and OS junk. Only
source, CMake files, scripts and assets are tracked (11 files plus the raylib
submodule pointer).
