//------------------------------------------------------------------------------
// pong - second example game
//
// Exists to prove the layout scales: drop another folder under examples/ and it
// shows up as its own target with no changes to any CMakeLists.txt.
//
// Controls : [W]/[S] left paddle, [Up]/[Down] right paddle
//            [Space] to serve, [R] to reset, [Esc] or close to quit
// Goal     : first to 7 points wins
//------------------------------------------------------------------------------

#include "raylib.h"
#include "raymath.h"

namespace
{
constexpr int   kScreenWidth  = 960;
constexpr int   kScreenHeight = 540;

constexpr float kPaddleWidth  = 14.0f;
constexpr float kPaddleHeight = 90.0f;
constexpr float kPaddleSpeed  = 420.0f;  // pixels per second
constexpr float kBallRadius   = 9.0f;
constexpr float kBallSpeed    = 380.0f;
constexpr float kBallSpeedMax = 720.0f;
constexpr int   kHudHeight    = 60;
constexpr int   kWinScore     = 7;

struct Paddle
{
    Rectangle bounds{ 0.0f, 0.0f, kPaddleWidth, kPaddleHeight };
    int       score = 0;
};

void MovePaddle(Paddle& paddle, float delta, float dt)
{
    const float top    = static_cast<float>(kHudHeight);
    const float bottom = static_cast<float>(kScreenHeight) - kPaddleHeight;
    paddle.bounds.y = Clamp(paddle.bounds.y + delta * kPaddleSpeed * dt, top, bottom);
}
}

int main()
{
    InitWindow(kScreenWidth, kScreenHeight, "raylib_learn - pong");
    SetTargetFPS(60);

    const float centerY = (kScreenHeight + kHudHeight) * 0.5f;

    Paddle left{};
    left.bounds = { 28.0f, centerY - kPaddleHeight * 0.5f, kPaddleWidth, kPaddleHeight };

    Paddle right{};
    right.bounds = { static_cast<float>(kScreenWidth) - 28.0f - kPaddleWidth,
                     centerY - kPaddleHeight * 0.5f, kPaddleWidth, kPaddleHeight };

    Vector2 ball     = { kScreenWidth * 0.5f, centerY };
    Vector2 ballVel  = { 0.0f, 0.0f };
    bool    serving  = true;   // waiting for [Space]
    bool    matchOver = false;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        if (IsKeyPressed(KEY_R))
        {
            left.score = 0;
            right.score = 0;
            left.bounds.y  = centerY - kPaddleHeight * 0.5f;
            right.bounds.y = centerY - kPaddleHeight * 0.5f;
            ball    = { kScreenWidth * 0.5f, centerY };
            ballVel = { 0.0f, 0.0f };
            serving   = true;
            matchOver = false;
        }

        if (!matchOver)
        {
            // --- paddles ------------------------------------------------------
            float leftDelta = 0.0f;
            if (IsKeyDown(KEY_W)) leftDelta -= 1.0f;
            if (IsKeyDown(KEY_S)) leftDelta += 1.0f;
            MovePaddle(left, leftDelta, dt);

            float rightDelta = 0.0f;
            if (IsKeyDown(KEY_UP))   rightDelta -= 1.0f;
            if (IsKeyDown(KEY_DOWN)) rightDelta += 1.0f;
            MovePaddle(right, rightDelta, dt);

            // --- serve --------------------------------------------------------
            if (serving && IsKeyPressed(KEY_SPACE))
            {
                const float dirX = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
                const float dirY = static_cast<float>(GetRandomValue(-100, 100)) / 100.0f;
                ballVel = Vector2Scale(Vector2Normalize({ dirX, dirY }), kBallSpeed);
                serving = false;
            }

            // --- ball ---------------------------------------------------------
            if (!serving)
            {
                ball.x += ballVel.x * dt;
                ball.y += ballVel.y * dt;

                // top / bottom walls
                if (ball.y - kBallRadius < kHudHeight)
                {
                    ball.y = static_cast<float>(kHudHeight) + kBallRadius;
                    ballVel.y = fabsf(ballVel.y);
                }
                if (ball.y + kBallRadius > kScreenHeight)
                {
                    ball.y = static_cast<float>(kScreenHeight) - kBallRadius;
                    ballVel.y = -fabsf(ballVel.y);
                }

                // paddles: bounce back and speed up a little every rally
                if (CheckCollisionCircleRec(ball, kBallRadius, left.bounds) && (ballVel.x < 0.0f))
                {
                    ballVel.x = fabsf(ballVel.x) * 1.05f;
                    ball.x = left.bounds.x + left.bounds.width + kBallRadius;
                }
                if (CheckCollisionCircleRec(ball, kBallRadius, right.bounds) && (ballVel.x > 0.0f))
                {
                    ballVel.x = -fabsf(ballVel.x) * 1.05f;
                    ball.x = right.bounds.x - kBallRadius;
                }
                if (fabsf(ballVel.x) > kBallSpeedMax)
                {
                    ballVel.x = (ballVel.x < 0.0f) ? -kBallSpeedMax : kBallSpeedMax;
                }

                // --- score ----------------------------------------------------
                if (ball.x < -kBallRadius)
                {
                    right.score += 1;
                    ball = { kScreenWidth * 0.5f, centerY };
                    ballVel = { 0.0f, 0.0f };
                    serving = true;
                }
                else if (ball.x > kScreenWidth + kBallRadius)
                {
                    left.score += 1;
                    ball = { kScreenWidth * 0.5f, centerY };
                    ballVel = { 0.0f, 0.0f };
                    serving = true;
                }

                matchOver = (left.score >= kWinScore) || (right.score >= kWinScore);
            }
        }

        // --- draw -------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Color{ 30, 33, 40, 255 });

        DrawRectangleLines(0, kHudHeight, kScreenWidth, kScreenHeight - kHudHeight, Color{ 70, 76, 90, 255 });

        // dashed centre line
        for (int y = kHudHeight + 10; y < kScreenHeight; y += 30)
        {
            DrawRectangle(kScreenWidth / 2 - 2, y, 4, 16, Color{ 70, 76, 90, 255 });
        }

        DrawRectangleRec(left.bounds, SKYBLUE);
        DrawRectangleRec(right.bounds, Color{ 255, 161, 90, 255 });
        DrawCircleV(ball, kBallRadius, RAYWHITE);

        DrawText(TextFormat("%d", left.score), kScreenWidth / 2 - 90, 16, 34, SKYBLUE);
        DrawText(TextFormat("%d", right.score), kScreenWidth / 2 + 60, 16, 34, Color{ 255, 161, 90, 255 });

        if (matchOver)
        {
            const char* winner = (left.score > right.score) ? "LEFT PLAYER WINS!" : "RIGHT PLAYER WINS!";
            DrawText(winner, (kScreenWidth - MeasureText(winner, 34)) / 2, kScreenHeight / 2 - 40, 34, GOLD);
            DrawText("PRESS [R] TO PLAY AGAIN",
                     (kScreenWidth - MeasureText("PRESS [R] TO PLAY AGAIN", 22)) / 2,
                     kScreenHeight / 2 + 10, 22, RAYWHITE);
        }
        else if (serving)
        {
            const char* hint = "PRESS [SPACE] TO SERVE";
            DrawText(hint, (kScreenWidth - MeasureText(hint, 24)) / 2, kScreenHeight - 46, 24,
                     Fade(RAYWHITE, 0.8f));
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
