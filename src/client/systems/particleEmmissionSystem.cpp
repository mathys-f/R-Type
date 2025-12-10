#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"
#include <math.h>

float randf() {
    return (rand() % 1000) / 1000.0f;
}

void ParticleEmmissionSystem(ecs::Registry& reg,
                        ecs::SparseArray<Position> const& positions,
                        ecs::SparseArray<Velocity> const& velocities,
                        ecs::SparseArray<Particle> const& particles,
                        ecs::SparseArray<Bullet> const& bullets) {
    for (auto [idx, pos_opt, vel_opt, bullets_opt] : 
        ecs::indexed_zipper(positions, velocities, bullets)) {
        
        if (pos_opt && vel_opt && bullets_opt){
            for (int i = 0; i < PARTICLES; i++) {
                auto particle = reg.spawn_entity();
                float angle = randf() * 2.0f * 3.14159f;
                float r = randf() * 20.0f;
                float offsetX = cosf(angle) * r;
                float offsetY = sinf(angle) * r;
                float posX = pos_opt->x + offsetX + 25.0f;
                float posY = pos_opt->y + offsetY + 5.0f;
                reg.add_component(particle, Position{posX, posY});
                float speed = randf() * 3.0f + 1.0f;
                reg.add_component(particle, Velocity{cosf(angle) * speed, sinf(angle) * speed});
                reg.add_component(particle, Particle{0.0f, 0.5f});
            }
        }
    }
}