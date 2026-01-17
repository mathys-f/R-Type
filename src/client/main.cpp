#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "raylib.h"
#include "scenes_loaders.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <optional>

#ifdef PLATFORM_DESKTOP
    #include <GLFW/glfw3.h>
#endif

using namespace engn;

// Handles user resize of the window to maintain 16:10 aspect ratio
static void update_window_size(EngineContext& engine_ctx) {
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    static int s_last_width = GetScreenWidth();
    static int s_last_height = GetScreenHeight();

    const int k_current_width = GetScreenWidth();
    const int k_current_height = GetScreenHeight();

    // Check if window was resized
    if (k_current_width != s_last_width || k_current_height != s_last_height) {
        // Force 16:10 aspect ratio using platform-specific window resizing
        const float k_target_aspect_ratio = 16.0f / 10.0f;
        const float k_current_aspect_ratio = static_cast<float>(k_current_width) / static_cast<float>(k_current_height);

        // Only resize if aspect ratio differs significantly
        if (std::fabs(k_current_aspect_ratio - k_target_aspect_ratio) > 0.01f) {
            int new_width = 0;
            int new_height = 0;

            if (k_current_aspect_ratio > k_target_aspect_ratio) {
                // Window is too wide, adjust width to match height
                new_height = k_current_height;
                new_width = static_cast<int>(static_cast<float>(k_current_height) * k_target_aspect_ratio);
            } else {
                // Window is too tall, adjust height to match width
                new_width = k_current_width;
                new_height = static_cast<int>(static_cast<float>(k_current_width) / k_target_aspect_ratio);
            }

            // This changes the viewport size, not the window size
            SetWindowSize(new_width, new_height);

            // Update stored dimensions
            s_last_width = new_width;
            s_last_height = new_height;
        } else {
            // Update stored dimensions even if no resize needed
            s_last_width = k_current_width;
            s_last_height = k_current_height;
        }
    }

    engine_ctx.window_size.x = static_cast<float>(GetScreenWidth());
    engine_ctx.window_size.y = static_cast<float>(GetScreenHeight());
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

}

int main(void) {
    constexpr int k_target_fps = 60;
    constexpr unsigned char k_alpha_opaque = 255;

    srand(time(NULL));

    // Create engine context
    EngineContext engine_ctx;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(engine_ctx.window_size.x);
    const int k_height = static_cast<int>(engine_ctx.window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(k_width, k_height, "FTL-Type");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetTargetFPS(k_target_fps);

    // Load assets
    engine_ctx.assets_manager.load_music("battle_music", "assets/music/Battle_music.mp3");
    engine_ctx.assets_manager.load_music("menu_music", "assets/music/Menu_music.mp3");
    engine_ctx.assets_manager.load_sound("shoot_sound", "assets/music/Blaster2.mp3");

    // Add scene loaders
    engine_ctx.add_scene_loader("singleplayer_game", load_game_scene);
    engine_ctx.add_scene_loader("multiplayer_game", load_multiplayer_game_scene);
    engine_ctx.add_scene_loader("main_menu", load_main_menu_scene);
    engine_ctx.add_scene_loader("lobby", load_lobby_scene);
    engine_ctx.add_scene_loader("audio_settings", load_audio_settings_scene);
    engine_ctx.add_scene_loader("keyboard_settings", load_keyboard_settings_menu_scene);
    engine_ctx.add_scene_loader("gamepad_settings", load_gamepad_settings_menu_scene);
    engine_ctx.add_scene_loader("graphics_settings", load_graphics_settings_scene);
    engine_ctx.add_scene_loader("difficulty_settings", load_difficulty_settings_scene);
    engine_ctx.set_scene("main_menu"); // Game menu scene

    std::optional<Music> music = engine_ctx.assets_manager.get_asset<Music>("menu_music");
    if (music.has_value())
        PlayMusicStream(music.value());
    // Main game loop
    while (!WindowShouldClose() && !engine_ctx.should_quit) {
        engine_ctx.delta_time = GetFrameTime();

        // Handle music change
        const std::string &scene_name = engine_ctx.get_current_scene();
        if (scene_name == "" && engine_ctx.change_music) {
            engine_ctx.change_music = false;
            StopMusicStream(music.value());
            music = engine_ctx.assets_manager.get_asset<Music>("battle_music");
            PlayMusicStream(music.value());
        } else if (scene_name == "main_menu" && engine_ctx.change_music) {
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

        if (GetKeyPressed() == KEY_F5){
            engine_ctx.set_scene(engine_ctx.get_current_scene());
        }

        EndDrawing();
        update_window_size(engine_ctx);
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
