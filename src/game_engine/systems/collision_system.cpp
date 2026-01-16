#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr float k_bullet_radius = 20.0f;
constexpr float k_explosion_sprite_x = 114.0f;
constexpr float k_explosion_sprite_y = 18.0f;
constexpr float k_explosion_sprite_w = 17.0f;
constexpr float k_explosion_sprite_h = 16.0f;
constexpr float k_explosion_scale = 3.0f;
constexpr float k_explosion_frame_duration = 0.08f;
constexpr int k_explosion_total_frames = 5;
constexpr int k_collision_damage = 25;
constexpr float k_bullet_damage = 10;
} // namespace

void sys::collision_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::Bullet> const& bullets, ecs::SparseArray<cpnt::Enemy> const& enemies,
                           ecs::SparseArray<cpnt::Health> const& healths, ecs::SparseArray<cpnt::Player> const& players,
                           ecs::SparseArray<cpnt::Hitbox> const& hitboxes, ecs::SparseArray<cpnt::Bullet_shooter> const& bullets_shooter,
                           ecs::SparseArray<cpnt::Shooter> const& shooters, ecs::SparseArray<cpnt::Stats> const& stats) {
    std::vector<ecs::Entity> bullets_to_kill;
    auto& reg = ctx.registry;

    int points_gained = 0;
    int enemies_killed = 0;

    // Debug - show hitboxes
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
    for (auto [idx, pos_opt, hitbox_opt] : ecs::indexed_zipper(positions, hitboxes)) {
        if (pos_opt && hitbox_opt) {
            Rectangle rect = {pos_opt->x + hitbox_opt->offset_x, pos_opt->y + hitbox_opt->offset_y,
                              hitbox_opt->width, hitbox_opt->height};
            DrawRectangleLinesEx(rect, 2.0f, GREEN);
        }
    }

    // Get bullet entities
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            // Check against all enemies
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && hitbox_opt) {
                    Rectangle enemy_rect = {enemy_pos_opt->x + hitbox_opt->offset_x,
                                            enemy_pos_opt->y + hitbox_opt->offset_y, hitbox_opt->width,
                                            hitbox_opt->height};
                    Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

                    if (CheckCollisionCircleRec(bullet_pos, k_bullet_radius, enemy_rect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion, cpnt::Transform{bullet_pos_opt->x, bullet_pos_opt->y, 0.0f, 0.0f,
                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                        reg.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                  k_explosion_scale,
                                                                  0,
                                                                  "bulletExplosion"});
                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion,
                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                          k_explosion_frame_duration, 0, k_explosion_total_frames});
                        // DrawCircleV(bullet_pos, 20, RED); // Visual debug

                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp--;
                            if (health->hp <= 0) {
                                points_gained += 100;
                                enemies_killed += 1;
                                health->hp = 0;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    // Player - Enemy collision
    for (auto [player_idx, player_pos_opt, player_tag_opt, player_hitbox_opt] :
         ecs::indexed_zipper(positions, players, hitboxes)) {
        if (player_pos_opt && player_tag_opt && player_hitbox_opt) {
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, enemy_hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && enemy_hitbox_opt) {
                    Rectangle enemy_rect = {enemy_pos_opt->x + enemy_hitbox_opt->offset_x,
                                            enemy_pos_opt->y + enemy_hitbox_opt->offset_y, enemy_hitbox_opt->width,
                                            enemy_hitbox_opt->height};
                    Rectangle player_rect = {player_pos_opt->x + player_hitbox_opt->offset_x,
                                             player_pos_opt->y + player_hitbox_opt->offset_y, player_hitbox_opt->width,
                                             player_hitbox_opt->height};

                    if (CheckCollisionRecs(player_rect, enemy_rect)) {
                        // Handle player hit logic here
                        // For simplicity, just reduce enemy health
                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp = 0;
                        }

                        auto& health_player = reg.get_components<cpnt::Health>()[player_idx];
                        if (health_player) {
                            health_player->hp -= k_collision_damage; // Reduce player health
                        }
                    }
                }
            }
        }
    }

    // Shooter bullet - Player collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_shot_tag_opt] : ecs::indexed_zipper(positions, bullets_shooter)) {
        if (!bullet_shot_tag_opt)
            continue;
        if (bullet_pos_opt && bullet_shot_tag_opt) {
            for (auto [player_idx, player_pos_opt, player_tag_opt, player_hitbox_opt] :
                 ecs::indexed_zipper(positions, players, hitboxes)) {
                if (player_pos_opt && player_tag_opt && player_hitbox_opt) {
                    Rectangle player_rect = {player_pos_opt->x + player_hitbox_opt->offset_x,
                                             player_pos_opt->y + player_hitbox_opt->offset_y, player_hitbox_opt->width,
                                             player_hitbox_opt->height};
                    Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

                    if (CheckCollisionCircleRec(bullet_pos, k_bullet_radius, player_rect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion, cpnt::Transform{bullet_pos_opt->x, bullet_pos_opt->y, 0.0f, 0.0f,
                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                        reg.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                  k_explosion_scale,
                                                                  0,
                                                                  "bulletExplosion"});
                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion,
                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                          k_explosion_frame_duration, 0, k_explosion_total_frames});

                        auto& health_player = reg.get_components<cpnt::Health>()[player_idx];
                        if (health_player) {
                            health_player->hp -= k_bullet_damage; // Reduce player health
                        }
                        break;
                    }
                }
            }
        }
    }

    // Bullet - Shooter collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            for (auto [shooter_idx, shooter_pos_opt, shooter_tag_opt, health_opt, shooter_hitbox_opt] :
                 ecs::indexed_zipper(positions, shooters, healths, hitboxes)) {
                if (shooter_pos_opt && shooter_tag_opt && health_opt && health_opt->hp > 0 && shooter_hitbox_opt) {
                    Rectangle shooter_rect = {shooter_pos_opt->x + shooter_hitbox_opt->offset_x,
                                             shooter_pos_opt->y + shooter_hitbox_opt->offset_y, shooter_hitbox_opt->width,
                                             shooter_hitbox_opt->height};
                    Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

                    if (CheckCollisionCircleRec(bullet_pos, k_bullet_radius, shooter_rect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion, cpnt::Transform{bullet_pos_opt->x, bullet_pos_opt->y, 0.0f, 0.0f,
                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                        reg.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                  k_explosion_scale,
                                                                  0,
                                                                  "bulletExplosion"});
                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion,
                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                          k_explosion_frame_duration, 0, k_explosion_total_frames});

                        auto& health_shooter = reg.get_components<cpnt::Health>()[shooter_idx];
                        if (health_shooter) {
                            health_shooter->hp -= 1;
                            if (health_shooter->hp <= 0) {
                                points_gained += 150;
                                enemies_killed += 1;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    // Player - Shooter collision
    for (auto [player_idx, player_pos_opt, player_tag_opt, player_hitbox_opt] :
         ecs::indexed_zipper(positions, players, hitboxes)) {
        if (player_pos_opt && player_tag_opt && player_hitbox_opt) {
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, enemy_hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && enemy_hitbox_opt) {
                    Rectangle enemy_rect = {enemy_pos_opt->x + enemy_hitbox_opt->offset_x,
                                            enemy_pos_opt->y + enemy_hitbox_opt->offset_y, enemy_hitbox_opt->width,
                                            enemy_hitbox_opt->height};
                    Rectangle player_rect = {player_pos_opt->x + player_hitbox_opt->offset_x,
                                             player_pos_opt->y + player_hitbox_opt->offset_y, player_hitbox_opt->width,
                                             player_hitbox_opt->height};

                    if (CheckCollisionRecs(player_rect, enemy_rect)) {
                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp = 0;
                        }

                        auto& health_player = reg.get_components<cpnt::Health>()[player_idx];
                        if (health_player) {
                            health_player->hp -= k_collision_damage; // Reduce player health
                        }
                    }
                }
            }
        }
    }

    for (auto e : bullets_to_kill) {
        reg.kill_entity(e);
    }

    // Update stats
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            stat->score += points_gained;
            stat->kills += enemies_killed;
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
}
