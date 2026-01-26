#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"
#include "raylib.h"

using namespace engn;

namespace {
constexpr float k_bullet_radius = 20.0f;
constexpr int k_collision_damage = 25;
constexpr int k_player_bullet_damage = 1;
constexpr int k_shooter_bullet_damage = 10;
constexpr int k_points_enemy = 100;
constexpr int k_points_shooter = 150;
constexpr int k_points_boss = 500;
} // namespace

void sys::server_collision_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::Bullet> const& bullets,
                           ecs::SparseArray<cpnt::BulletShooter> const& bullets_shooter,
                           ecs::SparseArray<cpnt::Enemy> const& enemies,
                           ecs::SparseArray<cpnt::Shooter> const& shooters,
                           ecs::SparseArray<cpnt::Health> const& healths,
                           ecs::SparseArray<cpnt::Hitbox> const& hitboxes,
                           ecs::SparseArray<cpnt::Player> const& players,
                           ecs::SparseArray<cpnt::BossHitbox> const& boss_hitboxes) {
    std::vector<ecs::Entity> bullets_to_kill;
    auto& reg = ctx.registry;
    int points_gained = 0;
    int enemies_killed = 0;

    // Bullet - Enemy collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            // Check against all enemies
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && hitbox_opt) {
                    Rectangle enemy_rect = {enemy_pos_opt->x + hitbox_opt->offset_x,
                                            enemy_pos_opt->y + hitbox_opt->offset_y,
                                            hitbox_opt->width,
                                            hitbox_opt->height};
                    Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

                    if (CheckCollisionCircleRec(bullet_pos, k_bullet_radius, enemy_rect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp -= k_player_bullet_damage;
                            if (health->hp <= 0) {
                                health->hp = 0;
                                points_gained += k_points_enemy;
                                enemies_killed += 1;
                            }
                            // Mark entities as dirty for networking
                            reg.mark_dirty<cpnt::Health>(reg.entity_from_index(enemy_idx));
                        }
                        break;
                    }
                }
            }
        }
    }

    // Player - Enemy collision
    for (auto [player_idx, player_pos_opt, player_tag_opt, player_health_opt, player_hitbox_opt] :
         ecs::indexed_zipper(positions, players, healths, hitboxes)) {
        if (player_pos_opt && player_tag_opt && player_health_opt && player_hitbox_opt) {
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, enemy_health_opt, enemy_hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && enemy_health_opt && enemy_health_opt->hp > 0 && enemy_hitbox_opt) {
                    Rectangle enemy_rect = {enemy_pos_opt->x + enemy_hitbox_opt->offset_x,
                                            enemy_pos_opt->y + enemy_hitbox_opt->offset_y,
                                            enemy_hitbox_opt->width,
                                            enemy_hitbox_opt->height};
                    Rectangle player_rect = {player_pos_opt->x + player_hitbox_opt->offset_x,
                                             player_pos_opt->y + player_hitbox_opt->offset_y,
                                             player_hitbox_opt->width,
                                             player_hitbox_opt->height};

                    if (CheckCollisionRecs(player_rect, enemy_rect)) {
                        
                        // Damage the enemy (match solo behavior)
                        auto& enemy_health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (enemy_health) {
                            enemy_health->hp = 0;
                            reg.mark_dirty<cpnt::Health>(reg.entity_from_index(enemy_idx));
                        }

                        // Damage the player
                        auto& player_health = reg.get_components<cpnt::Health>()[player_idx];
                        if (player_health) {
                            player_health->hp -= k_collision_damage;
                            if (player_health->hp < 0) {
                                player_health->hp = 0;
                            }
                            reg.mark_dirty<cpnt::Health>(reg.entity_from_index(player_idx));
                            if (player_health->hp == 0) {
                                const auto& player = reg.get_components<cpnt::Player>()[player_idx];
                                if (player) {
                                    ctx.dead_player_ids.insert(player->id);
                                }
                                reg.kill_entity_deferred(reg.entity_from_index(player_idx));
                            }
                        }
                    }
                }
            }
        }
    }

    // Shooter bullet - Player collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets_shooter)) {
        if (!bullet_pos_opt || !bullet_tag_opt) {
            continue;
        }
        for (auto [player_idx, player_pos_opt, player_tag_opt, player_hitbox_opt] :
             ecs::indexed_zipper(positions, players, hitboxes)) {
            if (!player_pos_opt || !player_tag_opt || !player_hitbox_opt) {
                continue;
            }

            Rectangle player_rect = {player_pos_opt->x + player_hitbox_opt->offset_x,
                                     player_pos_opt->y + player_hitbox_opt->offset_y,
                                     player_hitbox_opt->width,
                                     player_hitbox_opt->height};
            Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

            if (CheckCollisionCircleRec(bullet_pos, k_bullet_radius, player_rect)) {
                bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                auto& player_health = reg.get_components<cpnt::Health>()[player_idx];
                if (player_health) {
                    player_health->hp -= k_shooter_bullet_damage;
                    if (player_health->hp < 0) {
                        player_health->hp = 0;
                    }
                    reg.mark_dirty<cpnt::Health>(reg.entity_from_index(player_idx));
                    if (player_health->hp == 0) {
                        const auto& player = reg.get_components<cpnt::Player>()[player_idx];
                        if (player) {
                            ctx.dead_player_ids.insert(player->id);
                        }
                        reg.kill_entity_deferred(reg.entity_from_index(player_idx));
                    }
                }
                break;
            }
        }
    }

    // Bullet - Shooter collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets)) {
        if (!bullet_pos_opt || !bullet_tag_opt) {
            continue;
        }
        for (auto [shooter_idx, shooter_pos_opt, shooter_tag_opt, health_opt, hitbox_opt] :
             ecs::indexed_zipper(positions, shooters, healths, hitboxes)) {
            if (!shooter_pos_opt || !shooter_tag_opt || !health_opt || health_opt->hp <= 0 || !hitbox_opt) {
                continue;
            }
            Rectangle shooter_rect = {shooter_pos_opt->x + hitbox_opt->offset_x,
                                      shooter_pos_opt->y + hitbox_opt->offset_y,
                                      hitbox_opt->width,
                                      hitbox_opt->height};
            Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

            if (CheckCollisionCircleRec(bullet_pos, k_bullet_radius, shooter_rect)) {
                bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                auto& health = reg.get_components<cpnt::Health>()[shooter_idx];
                if (health) {
                    health->hp -= k_player_bullet_damage;
                    if (health->hp <= 0) {
                        health->hp = 0;
                        points_gained += k_points_shooter;
                        enemies_killed += 1;
                    }
                    reg.mark_dirty<cpnt::Health>(reg.entity_from_index(shooter_idx));
                }
                break;
            }
        }
    }

    // Player - Shooter collision
    for (auto [player_idx, player_pos_opt, player_tag_opt, player_hitbox_opt] :
         ecs::indexed_zipper(positions, players, hitboxes)) {
        if (!player_pos_opt || !player_tag_opt || !player_hitbox_opt) {
            continue;
        }
        for (auto [shooter_idx, shooter_pos_opt, shooter_tag_opt, health_opt, hitbox_opt] :
             ecs::indexed_zipper(positions, shooters, healths, hitboxes)) {
            if (!shooter_pos_opt || !shooter_tag_opt || !health_opt || health_opt->hp <= 0 || !hitbox_opt) {
                continue;
            }
            Rectangle shooter_rect = {shooter_pos_opt->x + hitbox_opt->offset_x,
                                      shooter_pos_opt->y + hitbox_opt->offset_y,
                                      hitbox_opt->width,
                                      hitbox_opt->height};
            Rectangle player_rect = {player_pos_opt->x + player_hitbox_opt->offset_x,
                                     player_pos_opt->y + player_hitbox_opt->offset_y,
                                     player_hitbox_opt->width,
                                     player_hitbox_opt->height};

            if (CheckCollisionRecs(player_rect, shooter_rect)) {
                auto& shooter_health = reg.get_components<cpnt::Health>()[shooter_idx];
                if (shooter_health) {
                    shooter_health->hp = 0;
                    reg.mark_dirty<cpnt::Health>(reg.entity_from_index(shooter_idx));
                }

                auto& player_health = reg.get_components<cpnt::Health>()[player_idx];
                if (player_health) {
                    player_health->hp -= k_collision_damage;
                    if (player_health->hp < 0) {
                        player_health->hp = 0;
                    }
                    reg.mark_dirty<cpnt::Health>(reg.entity_from_index(player_idx));
                    if (player_health->hp == 0) {
                        const auto& player = reg.get_components<cpnt::Player>()[player_idx];
                        if (player) {
                            ctx.dead_player_ids.insert(player->id);
                        }
                        reg.kill_entity_deferred(reg.entity_from_index(player_idx));
                    }
                }
            }
        }
    }

    // Bullet - Boss collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets)) {
        if (!bullet_pos_opt || !bullet_tag_opt) {
            continue;
        }
        for (auto [boss_idx, boss_pos_opt, health_opt, boss_hitbox_opt] :
             ecs::indexed_zipper(positions, healths, boss_hitboxes)) {
            if (!boss_pos_opt || !health_opt || health_opt->hp <= 0 || !boss_hitbox_opt) {
                continue;
            }

            Vector2 bullet_pos = {bullet_pos_opt->x, bullet_pos_opt->y};

            Rectangle rect_1 = {boss_pos_opt->x + boss_hitbox_opt->offset_x_1,
                                boss_pos_opt->y + boss_hitbox_opt->offset_y_1,
                                boss_hitbox_opt->width_1, boss_hitbox_opt->height_1};
            Rectangle rect_2 = {boss_pos_opt->x + boss_hitbox_opt->offset_x_2,
                                boss_pos_opt->y + boss_hitbox_opt->offset_y_2,
                                boss_hitbox_opt->width_2, boss_hitbox_opt->height_2};
            Rectangle rect_3 = {boss_pos_opt->x + boss_hitbox_opt->offset_x_3,
                                boss_pos_opt->y + boss_hitbox_opt->offset_y_3,
                                boss_hitbox_opt->width_3, boss_hitbox_opt->height_3};

            const bool k_hit_1 = CheckCollisionCircleRec(bullet_pos, k_bullet_radius, rect_1);
            const bool k_hit_2 = CheckCollisionCircleRec(bullet_pos, k_bullet_radius, rect_2);
            const bool k_hit_3 = CheckCollisionCircleRec(bullet_pos, k_bullet_radius, rect_3);

            if (k_hit_1 || k_hit_2 || k_hit_3) {
                bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));
                auto& boss_health = reg.get_components<cpnt::Health>()[boss_idx];
                if (boss_health) {
                    boss_health->hp -= k_player_bullet_damage;
                    if (boss_health->hp <= 0) {
                        boss_health->hp = 0;
                        points_gained += k_points_boss;
                        enemies_killed += 1;
                    }
                    reg.mark_dirty<cpnt::Health>(reg.entity_from_index(boss_idx));
                }
                break;
            }
        }
    }

    for (auto e : bullets_to_kill) {
        reg.kill_entity_deferred(e);
    }

    if (points_gained > 0 || enemies_killed > 0) {
        auto& stats = reg.get_components<cpnt::Stats>();
        for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
            if (!stats_opt) {
                continue;
            }
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            stat->score += points_gained;
            stat->kills += enemies_killed;
            reg.mark_dirty<cpnt::Stats>(reg.entity_from_index(stats_idx));
        }
    }
}
