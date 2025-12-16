#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <random>
#include <optional>

#include "raylib.h"
#include "game_engine/engine.h"
#include "game_engine/components/components.h"

using namespace engn;

static float randf() {
    return (rand() % 1000) / 1000.0f;
}

int main(void)
{
    srand(time(NULL));
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.1f, 0.8f);

    // Create engine context
    EngineContext engine_ctx(false);

    const int k_width = engine_ctx.window_size.x;
    const int k_height = engine_ctx.window_size.y;

    InitWindow(k_width, k_height, "FTL-Type");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Load assets
    engine_ctx.assets_manager.load_music("battle_music", "assets/music/Battle_music.mp3");
    engine_ctx.assets_manager.load_sound("shoot_sound", "assets/music/Blaster2.mp3");
    
    // Create player
    Rectangle shipSourceRect = {166.0f, 0.0f, 33.0f, 18.0f};

    auto player = engine_ctx.registry.spawn_entity();
    engine_ctx.registry.add_component(player, cpnt::Transform{(float)k_width/2, (float)k_height/2, 0, 0, 0, 0, 0, 0, 1, 1, 1});
    engine_ctx.registry.add_component(player, cpnt::Player{});
    engine_ctx.registry.add_component(player, cpnt::Sprite{shipSourceRect, 3.0f, 0, "assets/sprites/r-typesheet1.gif"});
    engine_ctx.registry.add_component(player, cpnt::Health{100, 100});
    engine_ctx.registry.add_component(player, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    
    // Create stars
    for (int i = 0; i < engine_ctx.stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{
            (float)GetRandomValue(0, k_width),
            (float)GetRandomValue(0, k_height),
            0, 0, 0, 0, 1, 1, 1
        });
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }
    
    // Create enemies
    for (size_t i = 0; i < engine_ctx.max_enemies; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawnY = (float)GetRandomValue(100, k_height - 100);
        float spawnX = (float)GetRandomValue(k_width, k_width * 2);

        // Position
        engine_ctx.registry.add_component(enemy, engn::cpnt::Transform{spawnX, spawnY, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(enemy, cpnt::Velocity{-(3.0f + randf() * 5.0f), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, cpnt::Enemy{});
        engine_ctx.registry.add_component(enemy, cpnt::Sprite{{5.0f, 6.0f, 21.0f, 23.0f}, 5.0f, 0, "assets/sprites/r-typesheet5.gif"});
        engine_ctx.registry.add_component(enemy, cpnt::Health{3, 3});

        // Create a **new MovementPattern instance** for this enemy
        cpnt::MovementPattern pat;
        pat.speed = 201.f + randf() * 3.f;
        pat.amplitude = (float)GetRandomValue(1, 10);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        int pattern_nbr = GetRandomValue(0, 3);
        switch (pattern_nbr) {
            case 0:
                pat.type = cpnt::MovementPattern::PatternType::ZigZag;
                break;
            case 1:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
            case 2:
                pat.type = cpnt::MovementPattern::PatternType::Sine;
                break;
            case 3:
                pat.type = cpnt::MovementPattern::PatternType::Dive;
                break;
        }
        pat.baseY = spawnY;

        engine_ctx.registry.add_component(enemy, std::move(pat));
    }

    std::optional<Music> battle_music = engine_ctx.assets_manager.get_asset<Music>("battle_music");
    if (battle_music.has_value())
        PlayMusicStream(battle_music.value());
    // Main game loop
    while (!WindowShouldClose() && !engine_ctx.should_quit)
    {
        engine_ctx.delta_time = GetFrameTime();
        
        // Run all systems
        engine_ctx.run_systems();

        // Update music
        if (battle_music.has_value())
            UpdateMusicStream(battle_music.value());
        
        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});
        
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








// int main()
// {
//     engn::EngineContext engine_ctx(false);

//     // Initialize Raylib
//     InitWindow(800, 600, "R-Type Client");
//     SetTargetFPS(60);

//     while (!WindowShouldClose() && !engine_ctx.should_quit) {
//         // Rendering
//         BeginDrawing();
//         ClearBackground(RAYWHITE);
//         engine_ctx.run_systems();
//         EndDrawing();
//     }

//     CloseWindow(); // Close window and OpenGL context

//     return 0;
// }