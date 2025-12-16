#include "components/components.h"
#include "engine.h"
#include "systems/systems.h"
#include "ecs/zipper.h"
#include <cmath>
#include <cstdlib>

using namespace engn;

static float randf() {
    return (rand() % 1000) / 1000.0f;
}

void sys::particle_emission_system(EngineContext &ctx,
                        ecs::SparseArray<cpnt::Transform> const& positions,
                        ecs::SparseArray<cpnt::Velocity> const& velocities,
                        ecs::SparseArray<cpnt::Particle> const& particles,
                        ecs::SparseArray<cpnt::Bullet> const& bullets)
{
    const int PARTICLES = ctx.particles;
    auto& reg = ctx.registry;

    for (auto [idx, pos_opt, vel_opt, bullets_opt] :
        ecs::indexed_zipper(positions, velocities, bullets)) {

        if (pos_opt && vel_opt && bullets_opt){
            for (int i = 0; i < PARTICLES; i++) {
                auto particle = reg.spawn_entity();
                float angle = randf() * 2.0f * 3.14159f;
                float r = randf() * 20.0f;
                float offsetX = std::cos(angle) * r;
                float offsetY = std::sin(angle) * r;
                float posX = pos_opt->x + offsetX + 25.0f;
                float posY = pos_opt->y + offsetY + 5.0f;
                reg.add_component(particle, cpnt::Transform{posX, posY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                float speed = randf() * 3.0f + 1.0f;
                reg.add_component(particle, cpnt::Velocity{std::cos(angle) * speed, std::sin(angle) * speed, 0.0f, 0.0f, 0.0f, 0.0f});
                reg.add_component(particle, cpnt::Particle{0.0f, 0.5f});
            }
        }
    }
}
