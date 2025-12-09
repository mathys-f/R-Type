#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"
#include <math.h>

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