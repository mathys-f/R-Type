#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr float k_bullet_radius = 20.0f;
constexpr int k_collision_damage = 1;
} // namespace

void sys::server_collision_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::Bullet> const& bullets, ecs::SparseArray<cpnt::Enemy> const& enemies,
                           ecs::SparseArray<cpnt::Health> const& healths,
                           ecs::SparseArray<cpnt::Hitbox> const& hitboxes,
                           ecs::SparseArray<cpnt::Player> const& players) {
    std::vector<ecs::Entity> bullets_to_kill;
    auto& reg = ctx.registry;

    // Bullet - Enemy collision
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            // Check against all enemies
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && hitbox_opt) {
                    // Simple rectangular collision check
                    float enemy_left = enemy_pos_opt->x + hitbox_opt->offset_x;
                    float enemy_right = enemy_left + hitbox_opt->width;
                    float enemy_top = enemy_pos_opt->y + hitbox_opt->offset_y;
                    float enemy_bottom = enemy_top + hitbox_opt->height;

                    float bullet_left = bullet_pos_opt->x - k_bullet_radius;
                    float bullet_right = bullet_pos_opt->x + k_bullet_radius;
                    float bullet_top = bullet_pos_opt->y - k_bullet_radius;
                    float bullet_bottom = bullet_pos_opt->y + k_bullet_radius;

                    // Check collision
                    if (bullet_right >= enemy_left && bullet_left <= enemy_right &&
                        bullet_bottom >= enemy_top && bullet_top <= enemy_bottom) {
                        LOG_INFO("[SERVER] Destroying bullet entity {} (hit enemy)", static_cast<std::uint32_t>(reg.entity_from_index(bullet_idx)));
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));

                        // Mark entities as dirty for networking
                        reg.mark_dirty<cpnt::Health>(reg.entity_from_index(enemy_idx));

                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp--;
                            if (health->hp <= 0) {
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
    for (auto [player_idx, player_pos_opt, player_tag_opt, player_health_opt, player_hitbox_opt] :
         ecs::indexed_zipper(positions, players, healths, hitboxes)) {
        if (player_pos_opt && player_tag_opt && player_health_opt && player_hitbox_opt) {
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, enemy_health_opt, enemy_hitbox_opt] :
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && enemy_health_opt && enemy_health_opt->hp > 0 && enemy_hitbox_opt) {
                    float enemy_left = enemy_pos_opt->x + enemy_hitbox_opt->offset_x;
                    float enemy_right = enemy_left + enemy_hitbox_opt->width;
                    float enemy_top = enemy_pos_opt->y + enemy_hitbox_opt->offset_y;
                    float enemy_bottom = enemy_top + enemy_hitbox_opt->height;

                    float player_left = player_pos_opt->x + player_hitbox_opt->offset_x;
                    float player_right = player_left + player_hitbox_opt->width;
                    float player_top = player_pos_opt->y + player_hitbox_opt->offset_y;
                    float player_bottom = player_top + player_hitbox_opt->height;

                    // Check collision
                    if (player_right >= enemy_left && player_left <= enemy_right &&
                        player_bottom >= enemy_top && player_top <= enemy_bottom) {
                        
                        // Damage the player
                        auto& player_health = reg.get_components<cpnt::Health>()[player_idx];
                        if (player_health) {
                            player_health->hp -= k_collision_damage;
                            if (player_health->hp < 0) {
                                player_health->hp = 0;
                            }
                            reg.mark_dirty<cpnt::Health>(reg.entity_from_index(player_idx));
                        }
                    }
                }
            }
        }
    }

    for (auto e : bullets_to_kill) {
        reg.kill_entity_deferred(e);
    }
}
