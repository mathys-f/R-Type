// #include <stdlib.h>
// #include <time.h>
// #include <stdio.h>
// #include <math.h>
// #include <random>

#include "raylib.h"

// #define WIDTH               1920
// #define HEIGHT              1000
// #define SCROLL_SPEED        5
// #define PARTICLES           3
// #define STARS               1000
// #define MAX_BULLETS         100
// #define MAX_ENEMIES         8

// int main(void)
// {
//     srand(time(NULL));

//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<float> dist(0.1f, 0.8f);

//     InitWindow(WIDTH, HEIGHT, "FTL-Type");
//     InitAudioDevice();
//     SetTargetFPS(60);

//     Texture2D shipTexture = LoadTexture("assets/sprites/r-typesheet1.gif");
//     Texture2D enemiesTexture = LoadTexture("assets/sprites/r-typesheet5.gif");
//     Texture2D bulletExplosionTexture = LoadTexture("assets/sprites/r-typesheet43.gif");
//     Texture2D explosionTexture = LoadTexture("assets/sprites/r-typesheet44.gif");
//     Music battleMusic = LoadMusicStream("assets/music/Battle_music.mp3");
//     Sound shootSound = LoadSound("assets/music/Blaster2.mp3");

//     ecs::Registry registry;

//     // Create player
//     Rectangle shipSourceRect = {166.0f, 0.0f, 33.0f, 18.0f};

//     auto player = registry.spawn_entity();
//     registry.add_component(player, Position{(float)WIDTH/2, (float)HEIGHT/2, {0, 0}});
//     registry.add_component(player, Player{});
//     registry.add_component(player, Sprite{shipSourceRect, 3.0f, 0, shipTexture});
//     registry.add_component(player, Health{100, 100});
//     registry.add_component(player, Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});

//     // Create stars
//     for (int i = 0; i < STARS; i++) {
//         auto star = registry.spawn_entity();
//         registry.add_component(star, Position{
//             (float)GetRandomValue(0, WIDTH),
//             (float)GetRandomValue(0, HEIGHT)
//         });
//         registry.add_component(star, Star{randf()});
//     }

//     // Create enemies
//     for (int i = 0; i < MAX_ENEMIES; i++) {
//         auto enemy = registry.spawn_entity();

//         float spawnY = (float)GetRandomValue(100, HEIGHT - 100);
//         float spawnX = (float)GetRandomValue(WIDTH, WIDTH * 2);

//         // Position
//         registry.add_component(enemy, Position{spawnX, spawnY, {0.0f, 0.0f}});

//         // Velocity
//         registry.add_component(enemy, Velocity{-(3.0f + randf() * 5.0f), 0.0f, 0.0f, 0.0f, 0.0f});

//         // Other components
//         registry.add_component(enemy, Enemy{});
//         registry.add_component(enemy, Sprite{{5.0f, 6.0f, 21.0f, 23.0f}, 5.0f, 0, enemiesTexture});
//         registry.add_component(enemy, Health{3, 3});

//         // Create a **new MovementPattern instance** for this enemy
//         MovementPattern pat;
//         pat.speed = 201.f + randf() * 3.f;
//         pat.amplitude = (float)GetRandomValue(1, 10);
//         pat.frequency = dist(gen);
//         pat.timer = 1.f;
//         int pattern_nbr = GetRandomValue(0, 3);
//         switch (pattern_nbr) {
//             case 0:
//                 pat.type = PatternType::ZigZag;
//                 break;
//             case 1:
//                 pat.type = PatternType::Straight;
//                 break;
//             case 2:
//                 pat.type = PatternType::Sine;
//                 break;
//             case 3:
//                 pat.type = PatternType::Dive;
//                 break;
//         }
//         pat.baseY = spawnY;

//         registry.add_component(enemy, MovementPattern(pat)); // <-- separate instance per enemy
//     }

//     float dt = GetFrameTime();

//     // Register systems - order matches system function signatures
//     registry.add_system<Position, Star>(StarScrollSystem);
//     registry.add_system<Position, Player, Sprite, Velocity, Health>([&shipTexture, &explosionTexture, &shootSound, &dt](auto& reg, auto& positions, auto& players, auto& sprites, auto& velocity, auto& healths) {
//         PlayerControlSystem(reg, positions, players, sprites, velocity, healths, shipTexture, explosionTexture, shootSound, dt);
//     });
//     //registry.add_system<Position, Velocity, Bullet>(BulletSystem);

//     registry.add_system<Position, Velocity, Bullet>([&dt](auto& reg, auto& positions, auto& velocities, auto& bullets) {
//         BulletSystem(reg, positions, velocities, bullets, dt);
//     });

//     registry.add_system<Position, Velocity, Enemy, Health, Sprite>([&explosionTexture](auto& reg, auto& positions, auto& velocities, auto& enemies, auto& healths, auto& sprites) {
//         EnemySystem(reg, positions, velocities, enemies, healths, sprites, explosionTexture);
//     });
//     registry.add_system<Position, Explosion, Sprite>(ExplosionSystem);
//     registry.add_system<Position, Bullet, Enemy, Health, Player>([&bulletExplosionTexture](auto& reg, auto& positions, auto& bullets, auto& enemies, auto& healths, auto& players) {
//         CollisionSystem(reg, positions, bullets, enemies, healths, players, bulletExplosionTexture);
//     });

//     registry.add_system<Position, MovementPattern, Velocity>(
//         [&dt](auto& reg, auto& positions, auto& patterns, auto& velocity) {
//             EnemyMovementSystem(reg, positions, patterns, velocity, dt);
//         }
//     );

//     registry.add_system<Position, Velocity, Particle, Bullet>(ParticleEmmissionSystem);

//     PlayMusicStream(battleMusic);
//     // Main game loop
//     while (!WindowShouldClose())
//     {
//         dt = GetFrameTime();
//         // Run all systems
//         registry.run_systems();

//         // Update music
//         UpdateMusicStream(battleMusic);

//         BeginDrawing();
//         ClearBackground((Color){0, 0, 0, 255});

//         // Render system (called separately for texture access)
//         RenderSystem(registry,
//                     registry.get_components<Position>(),
//                     registry.get_components<Sprite>(),
//                     registry.get_components<Star>(),
//                     registry.get_components<Velocity>(),
//                     registry.get_components<Particle>());

//         EndDrawing();
//     }

//     UnloadMusicStream(battleMusic);
//     UnloadSound(shootSound);
//     UnloadTexture(shipTexture);
//     UnloadTexture(enemiesTexture);
//     UnloadTexture(bulletExplosionTexture);
//     CloseAudioDevice();
//     CloseWindow();

//     return 0;
// }


#include "game_engine/engine.h"

int main()
{
    engn::EngineContext engine_ctx(false);

    // Initialize Raylib
    InitWindow(800, 600, "R-Type Client");
    SetTargetFPS(60);

    while (!WindowShouldClose() && !engine_ctx.should_quit) {
        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        engine_ctx.run_systems();
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
