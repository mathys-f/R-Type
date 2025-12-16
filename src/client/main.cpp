#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <optional>

#include "raylib.h"
#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "scenes_loaders.h"

using namespace engn;

int main(void)
{
    srand(time(NULL));

    // Create engine context
    EngineContext engine_ctx;

    const int k_width = engine_ctx.window_size.x;
    const int k_height = engine_ctx.window_size.y;

    InitWindow(k_width, k_height, "FTL-Type");
    InitAudioDevice();
    SetMasterVolume(0.0f);
    SetTargetFPS(60);

    // Load assets
    engine_ctx.assets_manager.load_music("battle_music", "assets/music/Battle_music.mp3");
    engine_ctx.assets_manager.load_sound("shoot_sound", "assets/music/Blaster2.mp3");

    // Add scene loaders
    engine_ctx.add_scene_loader(0, load_game_scene);
    engine_ctx.add_scene_loader(1, load_main_menu_scene);
    engine_ctx.add_scene_loader(2, load_server_connection_scene);
    engine_ctx.set_scene(1); // Game menu scene

    std::optional<Music> battle_music = engine_ctx.assets_manager.get_asset<Music>("battle_music");
    if (battle_music.has_value())
        PlayMusicStream(battle_music.value());
    // Main game loop
    while (!WindowShouldClose() && !engine_ctx.should_quit)
    {
        engine_ctx.delta_time = GetFrameTime();

        // Update music
        if (battle_music.has_value())
            UpdateMusicStream(battle_music.value());

        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});

        // Run all systems
        engine_ctx.run_systems();

        if (GetKeyPressed() == KEY_R) {
            engine_ctx.set_scene(engine_ctx.get_current_scene());
        }

        EndDrawing();
    }

    if (battle_music.has_value())
        UnloadMusicStream(battle_music.value());
    std::optional<Sound> shoot_sound = engine_ctx.assets_manager.get_asset<Sound>("shoot_sound");
    if (shoot_sound.has_value())
        UnloadSound(shoot_sound.value());
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
