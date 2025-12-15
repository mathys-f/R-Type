#include "raylib.h"

#include "game_engine/engine.h"

int main()
{
    engn::EngineContext engine_ctx(false);

    // Initialize Raylib
    InitWindow(800, 600, "R-Type Client");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        engine_ctx.run_systems();
        DrawText("R-Type Client Running...", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}