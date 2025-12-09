#include "raylib.h"
#include "../ecs/registry.h"
#include "../ecs/zipper.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <random>

#define WIDTH               1920
#define HEIGHT              500
#define STARS               1000
#define SCROLL_SPEED        12
#define MAX_BULLETS         100
#define MAX_ENEMIES         8

// ========== COMPONENTS ==========

struct Position {
    float x, y;
    Vector2 origin;
};

struct Velocity {
    float x, y, z;
    float rx, ry, rz;
};

struct Sprite {
    Rectangle sourceRect;
    float scale;
    int frame;
};

struct Health {
    int current;
    int max;
};

struct Player {
    // Tag component
};

struct Enemy {
    // Tag component
};

struct Bullet {
    // Tag component
};

struct Star {
    float z; // depth for parallax
};

struct Hitbox {
    float width, height;
    float offsetX, offsetY;
};

enum class PatternType {
    Straight,
    Sine,
    ZigZag,
    Dive
};

struct MovementPattern {
    PatternType type;
    float speed;
    float amplitude;
    float frequency;
    float timer;
    float baseY;
};

// ========== HELPER FUNCTIONS ==========

float randf() {
    return (rand() % 1000) / 1000.0f;
}

// ========== SYSTEMS ==========

void StarScrollSystem(ecs::Registry& reg, 
                     ecs::SparseArray<Position> const& positions,
                     ecs::SparseArray<Star> const& stars) {
    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<Position>()[idx];
            
            if (pos) {
                pos->x -= SCROLL_SPEED * (star_opt->z / 1);
                
                if (pos->x <= 0) {
                    pos->x += WIDTH;
                    pos->y = GetRandomValue(0, HEIGHT);
                }
            }
        }
    }
}

void PlayerControlSystem(ecs::Registry& reg,
                        ecs::SparseArray<Position> const& positions,
                        ecs::SparseArray<Player> const& players,
                        ecs::SparseArray<Sprite> const& sprites,
                        ecs::SparseArray<Velocity> const& velocities,
                        Texture2D& shipTexture,
                        Sound& shootSound) {
    for (auto [idx, pos_opt, player_opt, sprite_opt, vel_opt] : ecs::indexed_zipper(positions, players, sprites, velocities)) {
        if (pos_opt && player_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<Position>()[idx];
            auto& sprite = reg.get_components<Sprite>()[idx];
            auto& vel = reg.get_components<Velocity>()[idx];
            
            if (pos) {
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) pos->x += 10.0f;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) pos->x -= 10.0f;
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) pos->y -= 10.0f;
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) pos->y += 10.0f;
                
                // Clamp position
                if (pos->x >= WIDTH - 99) pos->x = WIDTH - 99;
                if (pos->x <= 0) pos->x = 0;
                if (pos->y >= HEIGHT - 54) pos->y = HEIGHT - 54;
                if (pos->y <= 0) pos->y = 0;
                
                // Animate sprite
                if (sprite) {
                    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                        if (sprite->frame <= 5 && sprite->sourceRect.x != 232.0f) {
                            sprite->frame++;
                            sprite->sourceRect.x = 199.0f;
                        } else {
                            sprite->sourceRect.x = 232.0f;
                            sprite->frame = 0;
                        }
                    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                        if (sprite->frame <= 5 && sprite->sourceRect.x != 100.0f) {
                            sprite->frame++;
                            sprite->sourceRect.x = 133.0f;
                        } else {
                            sprite->sourceRect.x = 100.0f;
                            sprite->frame = 0;
                        }
                    } else {
                        if (sprite->frame <= 5 && (sprite->sourceRect.x == 100.0f || sprite->sourceRect.x == 133.0f)) {
                            sprite->frame ++;
                            sprite->sourceRect.x = 133.0f;
                        } else if (sprite->frame <= 5 && (sprite->sourceRect.x == 232.0f || sprite->sourceRect.x == 199.0f)) {
                            sprite->frame ++;
                            sprite->sourceRect.x = 199.0f;
                        } else {
                            sprite->sourceRect.x = 166.0f;
                            sprite->frame = 0;
                        }
                    }
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                        vel->z = -5.0f;
                    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                        vel->z = 5.0f;
                    } else {
                        vel->z = 0.0f;
                    }
                }
                
                // Shoot
                if (IsKeyPressed(KEY_SPACE)) {
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, Position{pos->x + 50, pos->y + 30});
                    reg.add_component(bullet, Velocity{15.0f, 0.0f});
                    reg.add_component(bullet, Bullet{});
                    reg.add_component(bullet, Sprite{{249.0f, 105.0f, 16.0f, 8.0f}, 2.0f});
                    PlaySound(shootSound);
                }
            }
        }
    }
}

void BulletSystem(ecs::Registry& reg,
                 ecs::SparseArray<Position> const& positions,
                 ecs::SparseArray<Velocity> const& velocities,
                 ecs::SparseArray<Bullet> const& bullets) {
    std::vector<ecs::Entity> to_kill;
    
    for (auto [idx, pos_opt, vel_opt, bullet_opt] : ecs::indexed_zipper(positions, velocities, bullets)) {
        if (pos_opt && vel_opt && bullet_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<Position>()[idx];
            
            if (pos && vel_opt) {
                pos->x += vel_opt->x;
                pos->y += vel_opt->y;
                
                if (pos->x > WIDTH || pos->x < 0 || pos->y > HEIGHT || pos->y < 0) {
                    to_kill.push_back(entity);
                }
            }
        }
    }
    
    for (auto e : to_kill) {
        reg.kill_entity(e);
    }
}

void EnemySystem(ecs::Registry& reg,
                ecs::SparseArray<Position> const& positions,
                ecs::SparseArray<Velocity> const& velocities,
                ecs::SparseArray<Enemy> const& enemies,
                ecs::SparseArray<Health> const& healths,
                ecs::SparseArray<Sprite> const& sprites) {
    for (auto [idx, pos_opt, vel_opt, enemy_opt, health_opt, spr_opt] : 
         ecs::indexed_zipper(positions, velocities, enemies, healths, sprites)) {
        if (pos_opt && vel_opt && enemy_opt && health_opt) {
            auto& pos = reg.get_components<Position>()[idx];
            auto& health = reg.get_components<Health>()[idx];
            auto& sprite = reg.get_components<Sprite>()[idx];
            auto& vel = reg.get_components<Velocity>()[idx];
            
            if (pos && vel_opt) {
                pos->x += vel_opt->x;
                pos->y += vel_opt->y;
                // Respawn if off screen or dead
                if (pos->x < -100 || (health && health->current <= 0)) {
                    pos->x = GetRandomValue(WIDTH, WIDTH * 1.5);
                    pos->y = GetRandomValue(100, HEIGHT -100);
                    if (health) {
                        health->current = health->max;
                    }
                }
                
                //Animate enemies

                if (vel_opt->y > 0) {
                    if (sprite->frame <= 15 && sprite->sourceRect.x != 137.0f) {
                        sprite->frame++;
                        sprite->sourceRect.x = 104.0f;
                    } else {
                        sprite->sourceRect.x = 137.0f;
                        sprite->frame = 0;
                    }
                } else if (vel_opt->y < 0) {
                    if (sprite->frame <= 15 && sprite->sourceRect.x != 5.0f) {
                        sprite->frame++;
                        sprite->sourceRect.x = 38.0f;
                    } else {
                        sprite->sourceRect.x = 5.0f;
                        sprite->frame = 0;
                    }
                } else {
                    if (sprite->frame <= 15 && (sprite->sourceRect.x == 5.0f || sprite->sourceRect.x == 38.0f)) {
                        sprite->frame ++;
                        sprite->sourceRect.x = 38.0f;
                    } else if (sprite->frame <= 15 && (sprite->sourceRect.x == 137.0f || sprite->sourceRect.x == 104.0f)) {
                        sprite->frame ++;
                        sprite->sourceRect.x = 104.0f;
                    } else {
                        sprite->sourceRect.x = 71.0f;
                        sprite->frame = 0;
                    }
                }

            }
        }
    }
}

void CollisionSystem(ecs::Registry& reg,
                    ecs::SparseArray<Position> const& positions,
                    ecs::SparseArray<Bullet> const& bullets,
                    ecs::SparseArray<Enemy> const& enemies,
                    ecs::SparseArray<Health> const& healths) {
    std::vector<ecs::Entity> bullets_to_kill;
    
    // Get bullet entities
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : 
         ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            // Check against all enemies
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt] : 
                 ecs::indexed_zipper(positions, enemies, healths)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->current > 0) {
                    Rectangle enemyRect = {enemy_pos_opt->x, enemy_pos_opt->y, 100, 100};
                    Vector2 bulletPos = {bullet_pos_opt->x, bullet_pos_opt->y};
                    
                    if (CheckCollisionCircleRec(bulletPos, 5, enemyRect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));
                        
                        auto& health = reg.get_components<Health>()[enemy_idx];
                        if (health) {
                            health->current--;
                        }
                        break;
                    }
                }
            }
        }
    }
    
    for (auto e : bullets_to_kill) {
        reg.kill_entity(e);
    }
}

void RenderSystem(ecs::Registry& reg, 
                 ecs::SparseArray<Position> const& positions,
                 ecs::SparseArray<Sprite> const& sprites,
                 ecs::SparseArray<Star> const& stars,
                 ecs::SparseArray<Velocity> const& velocities,
                 Texture2D& shipTexture,
                 Texture2D& enemyTexture) {
    // Render stars
    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            DrawPixel(pos_opt->x, pos_opt->y, WHITE);
        }
    }
    
    // Render everything with sprites
    for (auto [idx, pos_opt, sprite_opt, vel_opt] : ecs::indexed_zipper(positions, sprites, velocities)) {
        if (pos_opt && sprite_opt) {
            auto& enemies_arr = reg.get_components<Enemy>();
            bool is_enemy = idx < enemies_arr.size() && enemies_arr[idx].has_value();
            
            Texture2D* texture = is_enemy ? &enemyTexture : &shipTexture;
            
            float width = sprite_opt->sourceRect.width * sprite_opt->scale;
            float height = sprite_opt->sourceRect.height * sprite_opt->scale;
            
            DrawTexturePro(*texture, 
                          sprite_opt->sourceRect,
                          (Rectangle){pos_opt->x + sprite_opt->sourceRect.width, pos_opt->y + sprite_opt->sourceRect.height, width, height},
                          (Vector2){pos_opt->origin.x, pos_opt->origin.y}, 
                          vel_opt ? vel_opt->z : 0.0f,
                          WHITE);
        }
    }
}


void EnemyMovementSystem(ecs::Registry& reg,
                         ecs::SparseArray<Position> const& positions,
                         ecs::SparseArray<MovementPattern> const& patterns,
                         ecs::SparseArray<Velocity> const& velocity,
                         float dt)
{
    for (auto [idx, pos_opt, pat_opt] : ecs::indexed_zipper(positions, patterns)) {
        if (!pos_opt || !pat_opt) continue;

        auto& pos = reg.get_components<Position>()[idx].value();
        auto& pat = reg.get_components<MovementPattern>()[idx].value();
        auto& vel = reg.get_components<Velocity>()[idx].value();

        pat.timer += dt;

        vel.x = -(pat.speed * dt); // consistent motion

        switch (pat.type) {
            case PatternType::Sine:
                vel.y = 
                    sinf(pat.timer * pat.frequency * 2.f * 3.14159f) *
                    pat.amplitude;
                break;

            case PatternType::ZigZag: {
                float phase = fmod(pat.timer * pat.frequency, 2.f);
                int dir = (phase < 1.f) ? 1 : -1;
                vel.y = dir * pat.amplitude;
                break;
            }

            case PatternType::Straight:
                break;

            case PatternType::Dive:
                vel.y = sinf(pat.timer * pat.frequency * 2.f * 3.14159f) *
                        (pat.amplitude * 2.f);
                break;
        }
    }
}

// ========== MAIN ==========

int main(void)
{
    srand(time(NULL));
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.1f, 0.8f);

    InitWindow(WIDTH, HEIGHT, "FTL-Type");
    InitAudioDevice();
    SetTargetFPS(60);
    
    Texture2D shipTexture = LoadTexture("assets/sprites/r-typesheet1.gif");
    Texture2D enemiesTexture = LoadTexture("assets/sprites/r-typesheet5.gif");
    Music battleMusic = LoadMusicStream("assets/music/Battle_music.mp3");
    Sound shootSound = LoadSound("assets/music/Blaster2.mp3");

    ecs::Registry registry;
    
    // Create player
    Rectangle shipSourceRect = {166.0f, 0.0f, 33.0f, 18.0f};

    auto player = registry.spawn_entity();
    registry.add_component(player, Position{(float)WIDTH/2, (float)HEIGHT/2, {shipSourceRect.width, shipSourceRect.height}});
    registry.add_component(player, Player{});
    registry.add_component(player, Sprite{shipSourceRect, 3.0f, 0});
    registry.add_component(player, Health{100, 100});
    registry.add_component(player, Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    
    // Create stars
    for (int i = 0; i < STARS; i++) {
        auto star = registry.spawn_entity();
        registry.add_component(star, Position{
            (float)GetRandomValue(0, WIDTH),
            (float)GetRandomValue(0, HEIGHT)
        });
        registry.add_component(star, Star{randf()});
    }
    
    // Create enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        auto enemy = registry.spawn_entity();

        float spawnY = (float)GetRandomValue(100, HEIGHT - 100);
        float spawnX = (float)GetRandomValue(WIDTH, WIDTH * 2);

        // Position
        registry.add_component(enemy, Position{spawnX, spawnY, {0.0f, 0.0f}});

        // Velocity
        registry.add_component(enemy, Velocity{-(3.0f + randf() * 5.0f), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        registry.add_component(enemy, Enemy{});
        registry.add_component(enemy, Sprite{{5.0f, 6.0f, 20.0f, 23.0f}, 5.0f, 0});
        registry.add_component(enemy, Health{3, 3});

        // Create a **new MovementPattern instance** for this enemy
        MovementPattern pat;
        pat.speed = 201.f + randf() * 3.f;
        pat.amplitude = (float)GetRandomValue(1, 10);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        int pattern_nbr = GetRandomValue(0, 3);
        switch (pattern_nbr) {
            case 0:
                pat.type = PatternType::Sine;
                break;
            case 1:
                pat.type = PatternType::Sine;
                break;
            case 2:
                pat.type = PatternType::Dive;
                break;
            case 3:
                pat.type = PatternType::Straight;
                break;
        }
        pat.baseY = spawnY;

        registry.add_component(enemy, MovementPattern(pat)); // <-- separate instance per enemy
    }

    float dt = GetFrameTime();

    // Register systems - order matches system function signatures
    registry.add_system<Position, Star>(StarScrollSystem);
    registry.add_system<Position, Player, Sprite, Velocity>([&shipTexture, &shootSound](auto& reg, auto& positions, auto& players, auto& sprites, auto& velocity) {
        PlayerControlSystem(reg, positions, players, sprites, velocity, shipTexture, shootSound);
    });
    registry.add_system<Position, Velocity, Bullet>(BulletSystem);
    registry.add_system<Position, Velocity, Enemy, Health, Sprite>(EnemySystem);
    registry.add_system<Position, Bullet, Enemy, Health>(CollisionSystem);
    
    registry.add_system<Position, MovementPattern, Velocity>(
        [&dt](auto& reg, auto& positions, auto& patterns, auto& velocity) {
            EnemyMovementSystem(reg, positions, patterns, velocity, dt);
        }
    );

    PlayMusicStream(battleMusic);
    // Main game loop
    while (!WindowShouldClose())
    {
        dt = GetFrameTime();
        // Run all systems
        registry.run_systems();

        // Update music
        UpdateMusicStream(battleMusic);
        
        BeginDrawing();
        ClearBackground((Color){0, 0, 0, 255});
        
        // Render system (called separately for texture access)
        RenderSystem(registry, 
                    registry.get_components<Position>(),
                    registry.get_components<Sprite>(),
                    registry.get_components<Star>(),
                    registry.get_components<Velocity>(),
                    shipTexture,
                    enemiesTexture);
        

        EndDrawing();
    }
    
    UnloadMusicStream(battleMusic);
    UnloadSound(shootSound);
    UnloadTexture(shipTexture);
    UnloadTexture(enemiesTexture);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}