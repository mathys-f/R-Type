#include "game_engine/api/lua.h"
#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "game_engine/network_client.h"
#include "raylib.h"
#include "scenes_loaders.h"
#include "systems/client_systems.h"

#include <iostream>
#include <random>

using namespace engn;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor; // NOLINT(clang-analyzer-security.insecureAPI.rand)
}

void load_multiplayer_game_scene(engn::EngineContext& engine_ctx) {
    constexpr float k_dist_min = 0.1f;
    constexpr float k_dist_max = 0.8f;
    constexpr float k_ship_sprite_x = 166.0f;
    constexpr float k_ship_sprite_y = 0.0f;
    constexpr float k_ship_width = 33.0f;
    constexpr float k_ship_height = 18.0f;
    constexpr float k_ship_scale = 3.0f;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(engine_ctx.window_size.x);
    const int k_height = static_cast<int>(engine_ctx.window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

    auto& registry = engine_ctx.registry;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);

    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::Shooter>();
    registry.register_component<cpnt::Explosion>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Replicated>();
    registry.register_component<cpnt::Sprite>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Tag>();
    registry.register_component<cpnt::Transform>();
    registry.register_component<cpnt::Star>();
    registry.register_component<cpnt::Sprite>();
    registry.register_component<cpnt::UIButton>();
    registry.register_component<cpnt::UICheckbox>();
    registry.register_component<cpnt::UIDropdown>();
    registry.register_component<cpnt::UIFocusable>();
    registry.register_component<cpnt::UIInputField>();
    registry.register_component<cpnt::UIInteractable>();
    registry.register_component<cpnt::UINavigation>();
    registry.register_component<cpnt::UISlider>();
    registry.register_component<cpnt::UIStyle>();
    registry.register_component<cpnt::UIText>();
    registry.register_component<cpnt::UITransform>();

    // Net
    engine_ctx.add_system<>(sys::handle_snapshots_deltas_system);
    // IO
    engine_ctx.add_system<>(sys::fetch_inputs);
    engine_ctx.add_system<>(send_input_system);
    // engine_ctx.add_system<>(sys::log_inputs);
    // UI
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    // Client should NOT simulate bullets in multiplayer - server is authoritative
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::BulletShooter>(sys::BulletShooter_system);

    // SIM / Prediction
    // engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox, cpnt::BulletShooter, cpnt::Shooter, cpnt::Stats>(
    //     sys::collision_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Explosion, cpnt::Sprite>(sys::explosion_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Particle, cpnt::Bullet, cpnt::BulletShooter>(sys::particle_emission_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(
    //     sys::player_control_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle, cpnt::Stats, cpnt::Boss>(
        sys::render_system);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_game_pause_inputs);
    // engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity, cpnt::Shooter, cpnt::Player>(sys::shooter_movement_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Health, cpnt::Sprite, cpnt::Shooter, cpnt::Player>(sys::shooter_system);

    engine_ctx.assets_manager.load_texture("bulletExplosion", "assets/sprites/r-typesheet43.gif");
    engine_ctx.assets_manager.load_texture("explosion", "assets/sprites/r-typesheet44.gif");
    engine_ctx.assets_manager.load_texture("enemy_ship", "assets/sprites/r-typesheet5.gif");
    engine_ctx.assets_manager.load_texture("player_ship", "assets/sprites/r-typesheet1.gif");

    // Reset and create network client in engine context
    if (engine_ctx.network_client) {
        engine_ctx.network_client->disconnect();
    }
    engine_ctx.network_client = std::make_shared<engn::NetworkClient>();

    engine_ctx.network_client->set_on_login([&engine_ctx, &registry, k_width, k_height](bool success, uint32_t player_id) {
        if (success) {
            LOG_DEBUG("Connected!");
        } else {
            LOG_ERROR("Login failed! Cannot start game.");
            return;
        }
    });

    engine_ctx.network_client->set_on_reliable([&engine_ctx](const net::Packet& pkt) {
        if (pkt.header.m_command == static_cast<std::uint8_t>(net::CommandId::KServerEntityState)) { // Received snapshot
            WorldDelta delta = WorldDelta::deserialize(pkt.payload.data());
            engine_ctx.add_snapshot_delta(delta);
        }
    });

    const char* player_name = "Player1";

    LOG_INFO("Connecting to {}:{}...", engine_ctx.server_ip, engine_ctx.server_port);
    engine_ctx.network_client->connect(engine_ctx.server_ip.c_str(), engine_ctx.server_port, player_name);

    engine_ctx.add_system<>([&engine_ctx](engn::EngineContext& ctx) { 
        if (engine_ctx.network_client) {
            engine_ctx.network_client->poll(); 
        }
    });

    for (int i = 0; i < engine_ctx.k_stars; i++) {
        auto star = registry.spawn_entity();
        registry.add_component(star, engn::cpnt::Transform{(float)GetRandomValue(0, k_width),
                                                           (float)GetRandomValue(0, k_height), 0, 0, 0, 0, 1, 1, 1});
        registry.add_component(star, cpnt::Star{randf()});
    }
}
