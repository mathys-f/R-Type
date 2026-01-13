#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "raylib.h"
#include "scenes_loaders.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <optional>

using namespace engn;

int main(void) {
    constexpr int k_target_fps = 60;
    constexpr unsigned char k_alpha_opaque = 255;
    constexpr unsigned char k_settings_audio_scene_id = 5;

    srand(time(NULL));

    // Create engine context
    EngineContext engine_ctx;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(engine_ctx.k_window_size.x);
    const int k_height = static_cast<int>(engine_ctx.k_window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

    InitWindow(k_width, k_height, "FTL-Type");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetTargetFPS(k_target_fps);

    // Load assets
    engine_ctx.assets_manager.load_music("battle_music", "assets/music/Battle_music.mp3");
    engine_ctx.assets_manager.load_music("menu_music", "assets/music/Menu_music.mp3");
    engine_ctx.assets_manager.load_sound("shoot_sound", "assets/music/Blaster2.mp3");

    // Add scene loaders
    engine_ctx.add_scene_loader(0, load_game_scene);
    engine_ctx.add_scene_loader(1, load_main_menu_scene);
    engine_ctx.add_scene_loader(2, load_lobby_scene);
    engine_ctx.add_scene_loader(3, load_multiplayer_game_scene);
    engine_ctx.add_scene_loader(4, load_settings_menu_scene);
    engine_ctx.add_scene_loader(k_settings_audio_scene_id, load_settings_audio_scene);
    engine_ctx.set_scene(1); // Game menu scene

    std::optional<Music> music = engine_ctx.assets_manager.get_asset<Music>("menu_music");
    if (music.has_value())
        PlayMusicStream(music.value());
    // Main game loop
    while (!WindowShouldClose() && !engine_ctx.should_quit) {
        engine_ctx.delta_time = GetFrameTime();

        // Handle music change
        std::optional<unsigned int> scene_id = engine_ctx.get_current_scene();
        if (scene_id == 0 && engine_ctx.change_music) {
            engine_ctx.change_music = false;
            StopMusicStream(music.value());
            music = engine_ctx.assets_manager.get_asset<Music>("battle_music");
            PlayMusicStream(music.value());
        } else if (scene_id == 1 && engine_ctx.change_music) {
            engine_ctx.change_music = false;
            StopMusicStream(music.value());
            music = engine_ctx.assets_manager.get_asset<Music>("menu_music");
            PlayMusicStream(music.value());
        }

        // Update music
        if (music.has_value())
            UpdateMusicStream(music.value());

        BeginDrawing();
        ClearBackground((Color){0, 0, 0, k_alpha_opaque});

        // Run all systems
        engine_ctx.run_systems();

        if (GetKeyPressed() == KEY_R) {
            engine_ctx.set_scene(engine_ctx.get_current_scene());
        }

        EndDrawing();
    }

    if (music.has_value())
        UnloadMusicStream(music.value());
    std::optional<Sound> shoot_sound = engine_ctx.assets_manager.get_asset<Sound>("shoot_sound");
    if (shoot_sound.has_value())
        UnloadSound(shoot_sound.value());
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
