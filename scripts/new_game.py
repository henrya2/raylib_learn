#!/usr/bin/env python3
"""Create the scaffolding for a new game.

Usage:
    python scripts/new_game.py my_game

Creates:
    games/<name>/CMakeLists.txt
    games/<name>/src/main.cpp
    assets/<name>/.gitkeep

CMake discovers the games/ sub-directories automatically, so after running
this just re-run cmake (or let your IDE do it) and the new target shows up.
"""

import argparse
from pathlib import Path
import re
import sys

REPO_ROOT = Path(__file__).resolve().parent.parent
GAMES_DIR = REPO_ROOT / "games"
ASSETS_DIR = REPO_ROOT / "assets"

CMAKE_TEMPLATE = """\
get_filename_component(GAME_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_game(${GAME_NAME})
"""

MAIN_TEMPLATE = r"""//------------------------------------------------------------------------------
// @GAME_NAME@
//
// Created by scripts/new_game.py. Put this game's assets in assets/@GAME_NAME@/
// and load them with bare filenames - at startup we change into that folder.
//------------------------------------------------------------------------------

#include "raylib.h"

int main()
{
    InitWindow(960, 540, "@GAME_NAME@");
    SetTargetFPS(60);

    // GAME_ASSETS_DIR is the fixed path from the .exe folder to this game's own
    // assets folder. Anchor to the .exe folder first so the game works no matter
    // which directory it was launched from, then step into the assets folder.
    ChangeDirectory(GetApplicationDirectory());
    ChangeDirectory(GAME_ASSETS_DIR);

    // Assets in assets/@GAME_NAME@/ can now be loaded directly, e.g.:
    //   Sound coin = LoadSound("coin.wav");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("@GAME_NAME@", 20, 20, 24, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
"""


def main() -> int:
    parser = argparse.ArgumentParser(description="Scaffold a new raylib game.")
    parser.add_argument(
        "name",
        help="game name, e.g. my_game (start with a letter; letters, digits, underscores)",
    )
    args = parser.parse_args()

    name = args.name
    if not re.fullmatch(r"[A-Za-z][A-Za-z0-9_]*", name):
        parser.error("name must start with a letter and contain only letters, digits and underscores")

    game_dir = GAMES_DIR / name
    asset_dir = ASSETS_DIR / name
    if game_dir.exists() or asset_dir.exists():
        print(
            f"error: '{name}' already exists (games/{name} or assets/{name})",
            file=sys.stderr,
        )
        return 1

    (game_dir / "src").mkdir(parents=True)
    asset_dir.mkdir(parents=True)

    (game_dir / "CMakeLists.txt").write_text(CMAKE_TEMPLATE, encoding="utf-8")
    (game_dir / "src" / "main.cpp").write_text(
        MAIN_TEMPLATE.replace("@GAME_NAME@", name), encoding="utf-8"
    )
    (asset_dir / ".gitkeep").touch()

    print(f"Created game '{name}':")
    print(f"  games/{name}/CMakeLists.txt")
    print(f"  games/{name}/src/main.cpp")
    print(f"  assets/{name}/.gitkeep")
    print("Re-run cmake to pick up the new target.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
