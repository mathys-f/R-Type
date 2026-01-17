#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
    constexpr int k_level_to_appear = 4;
    constexpr float k_large_explosion_y = 99.0f;
    constexpr float k_large_explosion_w = 65.0f;
    constexpr float k_large_explosion_h = 64.0f;
    constexpr float k_large_explosion_scale = 2.0f;
    constexpr float k_explosion_frame_duration = 0.08f;
    constexpr int k_explosion_frames = 5;
    constexpr float k_explosion_sprite_x = 114.0f;
    constexpr float k_explosion_sprite_y = 18.0f;
    constexpr float k_explosion_sprite_w = 17.0f;
    constexpr float k_explosion_sprite_h = 16.0f;
    constexpr float k_explosion_scale = 3.0f;
    constexpr float k_roarThickness = 20.0f;
    constexpr float k_cooldown_1_duration = 3.0f;
    constexpr float k_cooldown_2_duration = 5.0f;

    constexpr float k_bullet_sprite_x = 249.f; // Adjust to your values
    constexpr float k_bullet_sprite_y = 105.0f;
    constexpr float k_bullet_width = 16.0f;
    constexpr float k_bullet_height = 8.0f;
    constexpr float k_bullet_scale = 1.0f;
    constexpr float k_bullet_speed = 300.0f;
} // namespace

void sys::boss_system(EngineContext& ctx, ecs::SparseArray<cpnt::Boss> const& boss, ecs::SparseArray<cpnt::Transform> const& positions,
                    ecs::SparseArray<cpnt::Stats> const& stats, ecs::SparseArray<cpnt::BossHitbox> const& boss_hitboxes,
                    ecs::SparseArray<cpnt::Enemy> const& enemies, ecs::SparseArray<cpnt::Shooter> const& shooters,
                    ecs::SparseArray<cpnt::Bullet_shooter> const& bullets_shooter, ecs::SparseArray<cpnt::Bullet> const& bullets,
                    ecs::SparseArray<cpnt::Health> const& healths) {
    std::vector<ecs::Entity> entity_to_kill;
    auto& reg = ctx.registry;
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(ctx.window_size.x);
    const int k_height = static_cast<int>(ctx.window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
    const float k_maxDist = sqrtf(k_width * k_width + k_height * k_height);

    // Check level
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            if (stat->level >= k_level_to_appear && stat->boss_active == false) {
                stat->boss_active = true;
                const float k_boss_sprite_x = 27.0f;
                const float k_boss_sprite_y = 861.0f;
                const float k_boss_sprite_width = 154.0f;
                const float k_boss_sprite_height = 203.0f;
                const float k_boss_scale = 5.0f;
                const float k_boss_hitbox_head_width = 111.f * k_boss_scale;
                const float k_boss_hitbox_head_height = 86.f * k_boss_scale;
                const float k_boss_hitbox_body_width = 76.f * k_boss_scale;
                const float k_boss_hitbox_body_height = 93.f * k_boss_scale;
                const float k_boss_hitbox_tail_width = 68.f * k_boss_scale;
                const float k_boss_hitbox_tail_height = 30.f * k_boss_scale;
                const float k_boss_hitbox_head_x_offset = 0.f;
                const float k_boss_hitbox_head_y_offset = 0.f;
                const float k_boss_hitbox_body_x_offset = 83.f * k_boss_scale;
                const float k_boss_hitbox_body_y_offset = 85.f * k_boss_scale;
                const float k_boss_hitbox_tail_x_offset = 55.f * k_boss_scale;
                const float k_boss_hitbox_tail_y_offset = 176.f * k_boss_scale;


                auto boss = ctx.registry.spawn_entity();
                ctx.registry.add_component(boss, cpnt::Transform{ctx.window_size.x - 400.f, 0.f, 0, 0, 0, 0, 1, 1, 1});
                ctx.registry.add_component(boss, cpnt::Boss{0.f, 0.f, 0.f, true, false, {1350.f, 400.f}, 0.f, 600.f});
                ctx.registry.add_component(
                    boss, cpnt::Sprite{{27.0f, 861.0f, 154.0f, 203.0f}, k_boss_scale, 0, "boss"});
                ctx.registry.add_component(boss, cpnt::Health{100, 100});
                ctx.registry.add_component(boss, cpnt::Velocity{0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
                ctx.registry.add_component(boss, cpnt::BossHitbox{
                    k_boss_hitbox_head_width, k_boss_hitbox_head_height, k_boss_hitbox_head_x_offset, k_boss_hitbox_head_y_offset,
                    k_boss_hitbox_body_width, k_boss_hitbox_body_height, k_boss_hitbox_body_x_offset, k_boss_hitbox_body_y_offset,
                    k_boss_hitbox_tail_width, k_boss_hitbox_tail_height, k_boss_hitbox_tail_x_offset, k_boss_hitbox_tail_y_offset
                });
            }

            for (auto [boss_idx, boss_opt] : ecs::indexed_zipper(boss)) {
                if (boss_opt) {
                    auto& boss_comp = reg.get_components<cpnt::Boss>()[boss_idx];
                    
                    if (boss_comp->time_to_roar || boss_comp->roar_active) {
                        boss_comp->time_to_roar = false;
                        if (!boss_comp->roar_active) {
                            boss_comp->roar_active = true;
                            boss_comp->waveRadius = 0.0f;
                        }

                        // UPDATE wave radius
                        if (boss_comp->roar_active) {
                            boss_comp->waveRadius += boss_comp->waveSpeed * GetFrameTime();

                            if (boss_comp->waveRadius > k_maxDist) {
                                boss_comp->roar_active = false;
                            }
                        }

                        // Destroy enemies and bullets in wave radius
                        for (auto [pos_idx, pos_opt, charg_opt, shot_opt, bul_shot_opt, bul_opt] : ecs::indexed_zipper(positions, enemies, shooters, bullets_shooter, bullets)) {
                            if (pos_opt && (charg_opt || shot_opt || bul_shot_opt || bul_opt)) {
                                auto& pos = reg.get_components<cpnt::Transform>()[pos_idx];

                                float distX = pos->x - boss_comp->waveCenter.x;
                                float distY = pos->y - boss_comp->waveCenter.y;
                                float distance = sqrtf(distX * distX + distY * distY);

                                if (distance >= boss_comp->waveRadius - k_roarThickness && 
                                    distance <= boss_comp->waveRadius) {
                                    entity_to_kill.push_back(reg.entity_from_index(pos_idx));
                                    
                                    // Spawn explosion if charger or shooter
                                    if (charg_opt || shot_opt) {
                                        auto explosion = reg.spawn_entity();
                                        reg.add_component(explosion,
                                                          cpnt::Transform{pos->x, pos->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                                        reg.add_component(explosion, cpnt::Sprite{{0.0f, k_large_explosion_y, k_large_explosion_w,
                                                                                   k_large_explosion_h},
                                                                                  k_large_explosion_scale,
                                                                                  0,
                                                                                  "explosion"});
                                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                                        reg.add_component(explosion,
                                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                                    }

                                    if (bul_shot_opt || bul_opt) {
                                        auto explosion = reg.spawn_entity();
                                        reg.add_component(explosion, cpnt::Transform{pos_opt->x, pos_opt->y, 0.0f, 0.0f,
                                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                                        reg.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                                  k_explosion_scale,
                                                                                  0,
                                                                                  "bulletExplosion"});
                                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                                        reg.add_component(explosion,
                                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                                    }
                                }
                            }
                        }
                        
                        for (auto [pos_idx, pos_opt] : ecs::indexed_zipper(positions)) {
                            if (boss_comp->waveRadius > k_maxDist) {
                                boss_comp->roar_active = false;

                                // Cleanup: kill any remaining enemies/bullets that survived
                                auto& positions_cleanup = reg.get_components<cpnt::Transform>();
                                auto& enemies_cleanup = reg.get_components<cpnt::Enemy>();
                                auto& shooters_cleanup = reg.get_components<cpnt::Shooter>();
                                auto& bullets_cleanup = reg.get_components<cpnt::Bullet>();
                                auto& bullets_shooter_cleanup = reg.get_components<cpnt::Bullet_shooter>();

                                // Kill all chargers
                                for (auto [idx, pos_opt, enemy_opt] : ecs::indexed_zipper(positions_cleanup, enemies_cleanup)) {
                                    if (pos_opt && enemy_opt) {
                                        entity_to_kill.push_back(reg.entity_from_index(idx));
                                        auto explosion = reg.spawn_entity();
                                        reg.add_component(explosion,
                                                          cpnt::Transform{pos_opt->x, pos_opt->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                                        reg.add_component(explosion, cpnt::Sprite{{0.0f, k_large_explosion_y, k_large_explosion_w,
                                                                                   k_large_explosion_h},
                                                                                  k_large_explosion_scale,
                                                                                  0,
                                                                                  "explosion"});
                                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                                        reg.add_component(explosion,
                                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                                    }
                                }

                                // Kill all shooters
                                for (auto [idx, pos_opt, shooter_opt] : ecs::indexed_zipper(positions_cleanup, shooters_cleanup)) {
                                    if (pos_opt && shooter_opt) {
                                        entity_to_kill.push_back(reg.entity_from_index(idx));
                                        auto explosion = reg.spawn_entity();
                                        reg.add_component(explosion,
                                                          cpnt::Transform{pos_opt->x, pos_opt->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                                        reg.add_component(explosion, cpnt::Sprite{{0.0f, k_large_explosion_y, k_large_explosion_w,
                                                                                   k_large_explosion_h},
                                                                                  k_large_explosion_scale,
                                                                                  0,
                                                                                  "explosion"});
                                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                                        reg.add_component(explosion,
                                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                                    }
                                }

                                // Kill all bullets
                                for (auto [idx, pos_opt, bullet_opt] : ecs::indexed_zipper(positions_cleanup, bullets_cleanup)) {
                                    if (pos_opt && bullet_opt) {
                                        entity_to_kill.push_back(reg.entity_from_index(idx));
                                        auto explosion = reg.spawn_entity();
                                        reg.add_component(explosion, cpnt::Transform{pos_opt->x, pos_opt->y, 0.0f, 0.0f,
                                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                                        reg.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                                  k_explosion_scale,
                                                                                  0,
                                                                                  "bulletExplosion"});
                                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                                        reg.add_component(explosion,
                                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                                    }
                                }

                                // Kill all shooter bullets
                                for (auto [idx, pos_opt, bul_shot_opt] : ecs::indexed_zipper(positions_cleanup, bullets_shooter_cleanup)) {
                                    if (pos_opt && bul_shot_opt) {
                                        entity_to_kill.push_back(reg.entity_from_index(idx));
                                        auto explosion = reg.spawn_entity();
                                        reg.add_component(explosion, cpnt::Transform{pos_opt->x, pos_opt->y, 0.0f, 0.0f,
                                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                                        reg.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                                  k_explosion_scale,
                                                                                  0,
                                                                                  "bulletExplosion"});
                                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                                        reg.add_component(explosion,
                                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Check Health and remove boss if dead

    for (auto [boss_idx, boss_tag_opt, health_opt] : ecs::indexed_zipper(boss, healths)) {
        if (boss_tag_opt && health_opt) {
            if (health_opt->hp <= 0) {
                entity_to_kill.push_back(reg.entity_from_index(boss_idx));
            }
        }
    }

    for (auto e : entity_to_kill) {
        reg.kill_entity(e);
    }

    for (auto [boss_idx, boss_opt, pos_opt] : ecs::indexed_zipper(boss, positions)) {
        if (boss_opt && pos_opt && !boss_opt->roar_active) {
            auto& boss = reg.get_components<cpnt::Boss>()[boss_idx];
            auto& pos = reg.get_components<cpnt::Transform>()[boss_idx];
            
            float dt = GetFrameTime();
            boss->cooldown_1 -= dt;
            boss->cooldown_2 -= dt;
            
            // Attack 1: Semi-circle bullet spray from boss position
            if (boss->cooldown_1 <= 0.0f) {
                boss->cooldown_1 = k_cooldown_1_duration;
                
                // Spawn point (adjust based on your boss sprite)
                float spawn_x = pos->x + 400.0f; // Adjust offset
                float spawn_y = pos->y + 550.0f;
                
                // Create semi-circle of bullets (180 degrees, facing left/down)
                constexpr int num_bullets = 12;
                constexpr float start_angle = 90.0f;  // degrees
                constexpr float end_angle = 270.0f;
                
                for (int i = 0; i < num_bullets; i++) {
                    float angle = start_angle + (end_angle - start_angle) * i / (num_bullets - 1);
                    float rad = angle * DEG2RAD;
                    
                    float vx = cosf(rad) * k_bullet_speed;
                    float vy = sinf(rad) * k_bullet_speed;
                    
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Transform{spawn_x, spawn_y, 0.0f, 8.0f, 4.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                    reg.add_component(bullet, cpnt::Velocity{vx, vy, angle, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::Bullet_shooter{});
                    reg.add_component(bullet, cpnt::Hitbox{16.0f, 8.0f, 0.f, 0.f});
                    reg.add_component(bullet, cpnt::Sprite{{k_bullet_sprite_x, k_bullet_sprite_y, k_bullet_width, k_bullet_height},
                                                           k_bullet_scale, 0, "shooter_bullet"});
                }
                
                std::optional<Sound> shoot_sound = ctx.assets_manager.get_asset<Sound>("shoot_sound");
                if (shoot_sound.has_value())
                    PlaySound(shoot_sound.value());
            }
            
            // Attack 2: Vertical wall of bullets from right side moving left
            if (boss->cooldown_2 <= 0.0f) {
                boss->cooldown_2 = k_cooldown_2_duration;
                
                const int k_height = static_cast<int>(ctx.window_size.y);
                const int k_width = static_cast<int>(ctx.window_size.x);
                
                // Spawn bullets along right edge
                constexpr int num_bullets = 25;
                constexpr float spacing = 100.0f; // Vertical spacing between bullets
                
                float start_y = (k_height - (num_bullets * spacing)) / 2.0f; // Center vertically
                
                for (int i = 0; i < num_bullets; i++) {
                    float spawn_x = k_width - 1.0f; // Just off right edge
                    float spawn_y = start_y + i * spacing;
                    
                    float vx = -k_bullet_speed; // Move left
                    float vy = 0.0f;
                    
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Transform{spawn_x, spawn_y, 0.0f, 8.0f, 4.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                    reg.add_component(bullet, cpnt::Velocity{vx, vy, 180.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::Bullet_shooter{});
                    reg.add_component(bullet, cpnt::Hitbox{16.0f, 8.0f, 0.f, 0.f});
                    reg.add_component(bullet, cpnt::Sprite{{k_bullet_sprite_x, k_bullet_sprite_y, k_bullet_width, k_bullet_height},
                                                           k_bullet_scale, 0, "shooter_bullet"});
                }
                
                std::optional<Sound> shoot_sound = ctx.assets_manager.get_asset<Sound>("shoot_sound");
                if (shoot_sound.has_value())
                    PlaySound(shoot_sound.value());
            }
        }
    }
}
