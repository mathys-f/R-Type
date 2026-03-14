#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

#include <cmath>

using namespace engn;

namespace {
constexpr float k_pi = 3.14159f;
constexpr float k_rotation_offset = 180.0f;
} // namespace

void sys::server_shooter_movement_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                         ecs::SparseArray<cpnt::MovementPattern> const& patterns,
                                         ecs::SparseArray<cpnt::Velocity> const& velocity,
                                         ecs::SparseArray<cpnt::Shooter> const& shooters,
                                         ecs::SparseArray<cpnt::Player> const& players) {
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, pat_opt, vel_opt, shooter_opt] : ecs::indexed_zipper(positions, patterns, velocity, shooters)) {
        if (!pos_opt || !pat_opt || !vel_opt || !shooter_opt) {
            continue;
        }

        auto entity = reg.entity_from_index(idx);
        auto& pos = reg.get_components<cpnt::Transform>()[idx].value();
        auto& pat = reg.get_components<cpnt::MovementPattern>()[idx].value();
        auto& vel = reg.get_components<cpnt::Velocity>()[idx].value();

        pat.timer += dt;
        reg.mark_dirty<cpnt::MovementPattern>(entity);

        vel.vx = -(pat.speed * dt);
        reg.mark_dirty<cpnt::Velocity>(entity);

        float delta_x = 0.0f;
        float delta_y = 0.0f;
        bool has_player = false;
        for (auto [pidx, ppos_opt, player_opt] : ecs::indexed_zipper(positions, players)) {
            if (!ppos_opt || !player_opt) {
                continue;
            }
            auto& ppos = reg.get_components<cpnt::Transform>()[pidx].value();
            delta_x = ppos.x - pos.x;
            delta_y = ppos.y - pos.y;
            has_player = true;
            break;
        }

        if (has_player) {
            const float k_angle_to_player = std::atan2(delta_y, delta_x) * (180.0f / k_pi);
            vel.vz = k_angle_to_player + k_rotation_offset;
            reg.mark_dirty<cpnt::Velocity>(entity);
        }
    }
}
