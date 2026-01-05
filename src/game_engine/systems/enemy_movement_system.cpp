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

void sys::enemy_movement_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                ecs::SparseArray<cpnt::MovementPattern> const& patterns,
                                ecs::SparseArray<cpnt::Velocity> const& velocity) {
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, pat_opt] : ecs::indexed_zipper(positions, patterns)) {
        if (!pos_opt || !pat_opt)
            continue;

        auto& pos = reg.get_components<cpnt::Transform>()[idx].value();
        auto& pat = reg.get_components<cpnt::MovementPattern>()[idx].value();
        auto& vel = reg.get_components<cpnt::Velocity>()[idx].value();

        pat.timer += dt;

        vel.vx = -(pat.speed * dt); // consistent motion

        switch (pat.type) {
            case cpnt::MovementPattern::PatternType::Sine:
                vel.vy = std::sin(pat.timer * pat.frequency * k_two_pi) * pat.amplitude;
                break;

            case cpnt::MovementPattern::PatternType::ZigZag: {
                float phase = std::fmod(pat.timer * pat.frequency, k_zigzag_period);
                int dir = (phase < 1.f) ? 1 : -1;
                vel.vy = static_cast<float>(dir) * pat.amplitude;
                break;
            }

            case cpnt::MovementPattern::PatternType::Straight:
                break;

            case cpnt::MovementPattern::PatternType::Dive:
                vel.vy = std::sin(pat.timer * pat.frequency * k_two_pi) * (pat.amplitude * k_dive_amplitude_multiplier);
                break;
        }
    }
}
