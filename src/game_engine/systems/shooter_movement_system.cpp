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
                                ecs::SparseArray<cpnt::Shooter> const& shooters) {
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, pat_opt, shot_opt] : ecs::indexed_zipper(positions, patterns, shooters)) {
        if (!pos_opt || !pat_opt)
            continue;
        printf("Shooter Movement System Active\n");

        auto& pos = reg.get_components<cpnt::Transform>()[idx].value();
        auto& pat = reg.get_components<cpnt::MovementPattern>()[idx].value();
        auto& vel = reg.get_components<cpnt::Velocity>()[idx].value();

        pat.timer += dt;

        vel.vx = -(pat.speed * dt); // consistent motion
        printf("Shooter Position X: %f\n", pos.x);
        printf("Shooter Velocity X: %f\n", vel.vx);
    }
}