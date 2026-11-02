//------------------------------------------------------------------------------
// minimal_game - "Gem Catcher"
//
// The reference skeleton every other game in examples/ copies:
//   examples/<game>/CMakeLists.txt  -> add_example_game(<game>)
//   examples/<game>/src/main.cpp    -> this file
//
// Controls : WASD / arrow keys to move, [R] to restart, [Esc] or close to quit
// Goal     : collect as many gems as you can before the timer runs out
//------------------------------------------------------------------------------

#include "raylib.h"
#include "raymath.h"

namespace
{
constexpr int   kScreenWidth  = 960;
constexpr int   kScreenHeight = 540;

constexpr float kPlayerRadius = 22.0f;
constexpr float kPlayerSpeed  = 340.0f;  // pixels per second
constexpr float kGemRadius    = 12.0f;
constexpr float kRoundTime    = 30.0f;   // seconds
constexpr int   kHudHeight    = 60;

struct Gem
{
    Vector2 position{ 0.0f, 0.0f };
    Color   color{ 255, 255, 255, 255 };
};

Gem SpawnGem()
{
    const int margin = static_cast<int>(kGemRadius) * 2;

    Gem gem;
    gem.position.x = static_cast<float>(GetRandomValue(margin, kScreenWidth - margin));
    gem.position.y = static_cast<float>(GetRandomValue(kHudHeight + margin, kScreenHeight - margin));
    gem.color      = ColorFromHSV(static_cast<float>(GetRandomValue(0, 359)), 0.75f, 0.95f);
    return gem;
}

Vector2 ReadMovement()
{
    Vector2 move{ 0.0f, 0.0f };

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) move.x += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) move.x -= 1.0f;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) move.y += 1.0f;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) move.y -= 1.0f;

    // Normalising keeps diagonal movement as fast as straight movement.
    return ((move.x != 0.0f) || (move.y != 0.0f)) ? Vector2Normalize(move) : move;
}
}

int main()
{
    InitWindow(kScreenWidth, kScreenHeight, "raylib_learn - minimal_game");
    SetTargetFPS(60);

    Vector2 player   = { kScreenWidth * 0.5f, (kScreenHeight + kHudHeight) * 0.5f };
    Gem     gem      = SpawnGem();
    int     score    = 0;
    float   timeLeft = kRoundTime;
    bool    finished = false;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        if (IsKeyPressed(KEY_R))
        {
            player   = { kScreenWidth * 0.5f, (kScreenHeight + kHudHeight) * 0.5f };
            gem      = SpawnGem();
            score    = 0;
            timeLeft = kRoundTime;
            finished = false;
        }

        if (!finished)
        {
            // --- update -------------------------------------------------------
            const Vector2 move = ReadMovement();
            player.x = Clamp(player.x + move.x * kPlayerSpeed * dt,
                             kPlayerRadius, static_cast<float>(kScreenWidth) - kPlayerRadius);
            player.y = Clamp(player.y + move.y * kPlayerSpeed * dt,
                             static_cast<float>(kHudHeight) + kPlayerRadius,
                             static_cast<float>(kScreenHeight) - kPlayerRadius);

            if (CheckCollisionCircles(player, kPlayerRadius, gem.position, kGemRadius))
            {
                gem = SpawnGem();
                score += 10;
            }

            timeLeft -= dt;
            if (timeLeft <= 0.0f)
            {
                timeLeft = 0.0f;
                finished = true;
            }
        }

        // --- draw -------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Color{ 30, 33, 40, 255 });

        DrawRectangleLines(0, kHudHeight, kScreenWidth, kScreenHeight - kHudHeight, Color{ 70, 76, 90, 255 });

        DrawCircleV(gem.position, kGemRadius, gem.color);
        DrawCircleLines(static_cast<int>(gem.position.x), static_cast<int>(gem.position.y),
                        kGemRadius + 4.0f, Fade(gem.color, 0.4f));

        DrawCircleV(player, kPlayerRadius, SKYBLUE);
        DrawCircleLines(static_cast<int>(player.x), static_cast<int>(player.y),
                        kPlayerRadius, Color{ 135, 206, 250, 255 });

        DrawText(TextFormat("SCORE %d", score), 20, 16, 26, RAYWHITE);

        const char* timeText = TextFormat("TIME %.1f", timeLeft);
        DrawText(timeText, kScreenWidth - MeasureText(timeText, 26) - 20, 16, 26,
                 (timeLeft < 5.0f) ? RED : RAYWHITE);

        if (finished)
        {
            const char* over = "TIME UP - PRESS [R] TO PLAY AGAIN";
            DrawText(over, (kScreenWidth - MeasureText(over, 30)) / 2, kScreenHeight / 2 - 30, 30, GOLD);
        }
        else
        {
            DrawText("WASD / arrows to move, [R] to restart",
                     20, kScreenHeight - 30, 18, Fade(RAYWHITE, 0.6f));
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
