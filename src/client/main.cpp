#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <random>

#include "raylib.h"
#include "game_engine/engine.h"
#include "game_engine/components/components.h"

using namespace engn;
using namespace engn::cpnt;

#define WIDTH               1920
#define HEIGHT              1000

float randf() {
    return (rand() % 1000) / 1000.0f;
}

int main(void)
{
    srand(time(NULL));
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.1f, 0.8f);

    InitWindow(WIDTH, HEIGHT, "FTL-Type");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Load assets
    Music battleMusic = LoadMusicStream("assets/music/Battle_music.mp3");
    Sound shootSound = LoadSound("assets/music/Blaster2.mp3");

    // Create engine context
    EngineContext engine_ctx(false);
    
    // Create player
    Rectangle shipSourceRect = {166.0f, 0.0f, 33.0f, 18.0f};

    auto player = engine_ctx.registry.spawn_entity();
    engine_ctx.registry.add_component(player, engn::cpnt::Transform{(float)WIDTH/2, (float)HEIGHT/2, 0, 0, 0, 0, 1, 1, 1});
    engine_ctx.registry.add_component(player, Player{});
    engine_ctx.registry.add_component(player, Sprite{shipSourceRect, 3.0f, 0, "assets/sprites/r-typesheet1.gif"});
    engine_ctx.registry.add_component(player, Health{100, 100});
    engine_ctx.registry.add_component(player, Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    
    // Create stars
    for (int i = 0; i < engine_ctx.stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{
            (float)GetRandomValue(0, WIDTH),
            (float)GetRandomValue(0, HEIGHT),
            0, 0, 0, 0, 1, 1, 1
        });
        engine_ctx.registry.add_component(star, Star{randf()});
    }
    
    // Create enemies
    for (size_t i = 0; i < engine_ctx.max_enemies; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawnY = (float)GetRandomValue(100, HEIGHT - 100);
        float spawnX = (float)GetRandomValue(WIDTH, WIDTH * 2);

        // Position
        engine_ctx.registry.add_component(enemy, engn::cpnt::Transform{spawnX, spawnY, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(enemy, Velocity{-(3.0f + randf() * 5.0f), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, Enemy{});
        engine_ctx.registry.add_component(enemy, Sprite{{5.0f, 6.0f, 21.0f, 23.0f}, 5.0f, 0, "assets/sprites/r-typesheet5.gif"});
        engine_ctx.registry.add_component(enemy, Health{3, 3});

        // Create a **new MovementPattern instance** for this enemy
        MovementPattern pat;
        pat.speed = 201.f + randf() * 3.f;
        pat.amplitude = (float)GetRandomValue(1, 10);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        int pattern_nbr = GetRandomValue(0, 3);
        switch (pattern_nbr) {
            case 0:
                pat.type = MovementPattern::PatternType::ZigZag;
                break;
            case 1:
                pat.type = MovementPattern::PatternType::Straight;
                break;
            case 2:
                pat.type = MovementPattern::PatternType::Sine;
                break;
            case 3:
                pat.type = MovementPattern::PatternType::Dive;
                break;
        }
        pat.baseY = spawnY;

        engine_ctx.registry.add_component(enemy, std::move(pat));
    }

    PlayMusicStream(battleMusic);
    // Main game loop
    while (!WindowShouldClose())
    {
        engine_ctx.delta_time = GetFrameTime();
        
        // Run all systems
        engine_ctx.run_systems();

        // Update music
        UpdateMusicStream(battleMusic);
        
        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});
        
        EndDrawing();
    }
    
    UnloadMusicStream(battleMusic);
    UnloadSound(shootSound);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
