#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

#include <cmath>
#include <cstdlib>

using namespace engn;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
constexpr float k_pi = 3.14159f;
constexpr float k_two_pi = 2.0f * k_pi;
constexpr float k_particle_radius = 20.0f;
constexpr float k_particle_offset_x = 25.0f;
constexpr float k_particle_offset_y = 5.0f;
constexpr float k_particle_speed_multiplier = 3.0f;
constexpr float k_particle_speed_base = 1.0f;
constexpr float k_particle_lifetime = 0.5f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor; // NOLINT(clang-analyzer-security.insecureAPI.rand)
}

void sys::particle_emission_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                   ecs::SparseArray<cpnt::Velocity> const& velocities,
                                   ecs::SparseArray<cpnt::Particle> const& particles,
                                   ecs::SparseArray<cpnt::Bullet> const& bullets,
                                   ecs::SparseArray<cpnt::BulletShooter> const& bullets_shooter) {
    const auto k_particles = static_cast<int>(ctx.k_particles);
    auto& reg = ctx.registry;

    for (auto [idx, pos_opt, bullets_opt] : ecs::indexed_zipper(positions, bullets)) {

        if (pos_opt && bullets_opt) {
            auto pos = pos_opt.value();

            for (int i = 0; i < k_particles; i++) {
                auto particle = reg.spawn_entity();
                float angle = randf() * k_two_pi;
                float r = randf() * k_particle_radius;
                float offset_x = std::cos(angle) * r;
                float offset_y = std::sin(angle) * r;
                float pos_x = pos.x + offset_x + k_particle_offset_x;
                float pos_y = pos.y + offset_y + k_particle_offset_y;
                reg.add_component(particle, cpnt::Transform{pos_x, pos_y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                float speed = randf() * k_particle_speed_multiplier + k_particle_speed_base;
                reg.add_component(
                    particle, cpnt::Velocity{std::cos(angle) * speed, std::sin(angle) * speed, 0.0f, 0.0f, 0.0f, 0.0f});
                reg.add_component(particle, cpnt::Particle{0.0f, k_particle_lifetime, 0, 0, 255}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            }
        }
    }

    for (auto [idx, pos_opt, bullets_shot_opt] : ecs::indexed_zipper(positions, bullets_shooter)) {

        if (pos_opt && bullets_shot_opt) {
            auto pos = pos_opt.value();

            for (int i = 0; i < k_particles; i++) {
                auto particle = reg.spawn_entity();
                float angle = randf() * k_two_pi;
                float r = randf() * k_particle_radius;
                float offset_x = std::cos(angle) * r;
                float offset_y = std::sin(angle) * r;
                float pos_x = pos.x + offset_x + k_particle_offset_x;
                float pos_y = pos.y + offset_y + k_particle_offset_y;
                reg.add_component(particle, cpnt::Transform{pos_x, pos_y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                float speed = randf() * k_particle_speed_multiplier + k_particle_speed_base;
                reg.add_component(
                    particle, cpnt::Velocity{std::cos(angle) * speed, std::sin(angle) * speed, 0.0f, 0.0f, 0.0f, 0.0f});
                reg.add_component(particle, cpnt::Particle{0.0f, k_particle_lifetime, 255, 0, 0}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            }
        }
    }
}
