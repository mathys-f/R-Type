#include "game_engine/api/lua.h"
#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "game_engine/network_client.h"
#include "raylib.h"
#include "scenes_loaders.h"
#include "systems/client_systems.h"

#include <cstring>
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

    engine_ctx.game_over_retry_scene = "lobby";
    engine_ctx.pending_game_over.store(false);

    auto& registry = engine_ctx.registry;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);

    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::BulletShooter>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::Shooter>();
    registry.register_component<cpnt::Explosion>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Replicated>();
    registry.register_component<cpnt::EntityType>();
    registry.register_component<cpnt::Sprite>();
    registry.register_component<cpnt::Boss>();
    registry.register_component<cpnt::BossHitbox>();
    registry.register_component<cpnt::Particle>();
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

    // IO
    engine_ctx.add_system<>(sys::fetch_inputs);
    engine_ctx.add_system<>(sys::resolve_player_input);
    engine_ctx.add_system<>(send_input_system);
    // Prediction
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity>(sys::predict_local_player_system);
    // Net
    engine_ctx.add_system<>(sys::apply_server_updates_system);
    // engine_ctx.add_system<>(sys::log_inputs);
    // UI
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    // Client should NOT simulate bullets in multiplayer - server is authoritative
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::BulletShooter>(sys::BulletShooter_system);

    // SIM / Prediction
    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox, cpnt::BulletShooter, cpnt::Shooter, cpnt::Stats, cpnt::BossHitbox>(
        sys::collision_system);
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
    engine_ctx.assets_manager.load_texture("players", "assets/sprites/r-typesheet42.gif");
    engine_ctx.assets_manager.load_texture("shooter_sprite", "assets/sprites/r-typesheet19.gif");
    engine_ctx.assets_manager.load_texture("shooter_bullet", "assets/sprites/r-typesheet1_bis.gif");
    engine_ctx.assets_manager.load_texture("boss", "assets/sprites/r-typesheet30.gif");

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
        } else if (pkt.header.m_command == static_cast<std::uint8_t>(net::CommandId::kGameEnded)) {
            if (pkt.payload.size() < sizeof(int)) {
                LOG_WARNING("Received malformed game over packet: payload too small ({})", pkt.payload.size());
                return;
            }

            int score = 0;
            std::memcpy(&score, pkt.payload.data(), sizeof(score));
            engine_ctx.pending_game_over_score.store(score);
            engine_ctx.pending_game_over.store(true);
        }
    });
    
    engine_ctx.network_client->set_on_logout([&engine_ctx]() {
        LOG_INFO("Disconnected from server.");
        
        // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
        const float k_width = engine_ctx.window_size.x;
        const float k_height = engine_ctx.window_size.y;
        // NOLINTEND(cppcoreguidelines-pro-type-union-access)

        auto& registry = engine_ctx.registry;
        auto& tag_registry = registry.get_tag_registry();

        if (tag_registry.get_entity("disconnect_back_button").has_value()) {
            return;
        }

        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        auto bg_entity = registry.spawn_entity();
        registry.add_component(bg_entity, cpnt::UITransform{0, 0, 10, 100, 100, 0, 0, 0});
        registry.add_component(bg_entity, cpnt::UIStyle{
            utils::Color{0, 0, 0, 200}, utils::Color{0, 0, 0, 200}, utils::Color{0, 0, 0, 200},
            utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0},
            utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0},
            0, 0
        });
        registry.add_component(bg_entity, cpnt::UIInteractable{});

        auto text_entity = registry.spawn_entity();
        registry.add_component(text_entity, cpnt::UITransform{50, 40, 11, 0, 0, 0.5f, 0.5f, 0});
        registry.add_component(text_entity, cpnt::UIText{"Disconnected from server", 40});
        registry.add_component(text_entity, cpnt::UIStyle{
            utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0},
            utils::Color{255, 50, 50, 255}, utils::Color{255, 50, 50, 255}, utils::Color{255, 50, 50, 255},
            utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0}, utils::Color{0, 0, 0, 0},
            0, 0
        });

        auto btn_entity = registry.spawn_entity();
        tag_registry.create_and_bind_tag("disconnect_back_button", btn_entity);
        registry.add_component(btn_entity, cpnt::Tag{tag_registry.get_tag_id("disconnect_back_button")});
        
        registry.add_component(btn_entity, cpnt::UITransform{37.5f, 60, 11, 25, 8, 0.5f, 0.5f, 0});
        registry.add_component(btn_entity, cpnt::UIText{"Back to Menu", 30});
        registry.add_component(btn_entity, cpnt::UIStyle{
            utils::Color{50, 50, 50, 255}, utils::Color{70, 70, 70, 255}, utils::Color{30, 30, 30, 255},
            utils::Color{255, 255, 255, 255}, utils::Color{255, 255, 255, 255}, utils::Color{200, 200, 200, 255},
            utils::Color{100, 100, 100, 255}, utils::Color{120, 120, 120, 255}, utils::Color{80, 80, 80, 255},
            0.5f, 2.0f
        });
        registry.add_component(btn_entity, cpnt::UIButton{});
        registry.add_component(btn_entity, cpnt::UIInteractable{});
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    });

    const char* player_name = "Player1";

    LOG_INFO("Connecting to {}:{}...", engine_ctx.server_ip, engine_ctx.server_port);
    engine_ctx.network_client->connect(engine_ctx.server_ip.c_str(), engine_ctx.server_port, player_name);

    engine_ctx.add_system<>([&engine_ctx](engn::EngineContext& ctx) {
        if (engine_ctx.network_client) {
            engine_ctx.network_client->poll();
        }
    });

    engine_ctx.add_system<>([](engn::EngineContext& ctx) {
        if (!ctx.pending_game_over.load()) {
            return;
        }

        ctx.game_over_score = ctx.pending_game_over_score.load();
        ctx.pending_game_over.store(false);
        ctx.set_scene("game_over");
    });

    engine_ctx.add_system<>(handle_disconnect_ui_events);

    for (int i = 0; i < engine_ctx.k_stars; i++) {
        auto star = registry.spawn_entity();
        registry.add_component(star, engn::cpnt::Transform{(float)GetRandomValue(0, k_width),
                                                           (float)GetRandomValue(0, k_height), 0, 0, 0, 0, 1, 1, 1});
        registry.add_component(star, cpnt::Star{randf()});
    }
}
