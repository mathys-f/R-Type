#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

#include <cmath>

using namespace engn;

namespace {
constexpr float k_pi = 3.14159f;
constexpr float k_two_pi = 2.0f * k_pi;
constexpr float k_zigzag_period = 2.0f;
constexpr float k_dive_amplitude_multiplier = 2.0f;
} // namespace

void sys::shooter_movement_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                ecs::SparseArray<cpnt::MovementPattern> const& patterns,
                                ecs::SparseArray<cpnt::Velocity> const& velocity,
                                ecs::SparseArray<cpnt::Shooter> const& shooters,
                                ecs::SparseArray<cpnt::Player> const& player) {
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, posplay_opt, play_opt] : ecs::indexed_zipper(positions, player)) {
        if (!posplay_opt || !play_opt)
            continue;
        auto& posplay = reg.get_components<cpnt::Transform>()[idx].value();
        printf("Player position x: %f, y: %f\n", posplay.x, posplay.y);
    }

    for (auto [idx, pos_opt, pat_opt, shot_opt] : ecs::indexed_zipper(positions, patterns, shooters)) {
        if (!pos_opt || !pat_opt || !shot_opt)
            continue;

        auto& pos = reg.get_components<cpnt::Transform>()[idx].value();
        auto& pat = reg.get_components<cpnt::MovementPattern>()[idx].value();
        auto& vel = reg.get_components<cpnt::Velocity>()[idx].value();

        pat.timer += dt;

        vel.vx = -(pat.speed * dt); // consistent motion

        // Rotate shooter to face player
        float delta_x = 0.0f;
        float delta_y = 0.0f;
        for (auto [pidx, ppos_opt, pplay_opt] : ecs::indexed_zipper(positions, player)) {
            if (!ppos_opt || !pplay_opt)
                continue;
            auto& ppos = reg.get_components<cpnt::Transform>()[pidx].value();
            delta_x = ppos.x - pos.x;
            delta_y = ppos.y - pos.y;
            break; // Assuming only one player
        }
        float angle_to_player = std::atan2(delta_y, delta_x) * (180.0f / k_pi);
        vel.vz = angle_to_player + 180;
    }
}