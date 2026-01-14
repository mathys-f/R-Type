#include "game_engine/api/lua.h"
#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "raylib.h"
#include "scenes_loaders.h"
#include "systems/client_systems.h"

#include <random>

using namespace engn;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor;
}

void load_game_scene(engn::EngineContext& engine_ctx) {
    constexpr float k_dist_min = 0.1f;
    constexpr float k_dist_max = 0.8f;
    engine_ctx.input_context = InputContext::Gameplay;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(engine_ctx.k_window_size.x);
    const int k_height = static_cast<int>(engine_ctx.k_window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

    auto& registry = engine_ctx.registry;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);

    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::Enemy>();
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

    engine_ctx.add_system<>(sys::fetch_inputs);
    // engine_ctx.add_system<>(sys::log_inputs);
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox>(
        sys::collision_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Explosion, cpnt::Sprite>(sys::explosion_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Particle, cpnt::Bullet>(sys::particle_emission_system);
    engine_ctx.add_system<>(sys::resolve_player_input);
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(
        sys::player_control_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle>(
        sys::render_system);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_game_pause_inputs);

    // Load assets

    engine_ctx.assets_manager.load_texture("bulletExplosion", "assets/sprites/r-typesheet43.gif");
    engine_ctx.assets_manager.load_texture("explosion", "assets/sprites/r-typesheet44.gif");

    // Create player
    constexpr float k_ship_sprite_x = 166.0f;
    constexpr float k_ship_sprite_y = 0.0f;
    constexpr float k_ship_width = 33.0f;
    constexpr float k_ship_height = 18.0f;
    constexpr float k_ship_scale = 3.0f;
    constexpr int k_player_health = 100;

    Rectangle ship_source_rect = {k_ship_sprite_x, k_ship_sprite_y, k_ship_width, k_ship_height};

    engine_ctx.assets_manager.load_texture("player_ship", "assets/sprites/r-typesheet1.gif");

    auto player = engine_ctx.registry.spawn_entity();
    engine_ctx.registry.add_component(
        player, cpnt::Transform{(float)k_width / 2, (float)k_height / 2, 0, 0, 0, 0, 0, 0, 1, 1, 1});
    engine_ctx.registry.add_component(player, cpnt::Player{});
    engine_ctx.registry.add_component(player, cpnt::Sprite{ship_source_rect, k_ship_scale, 0, "player_ship"});
    engine_ctx.registry.add_component(player, cpnt::Health{k_player_health, k_player_health});
    engine_ctx.registry.add_component(player, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    engine_ctx.registry.add_component(
        player, cpnt::Hitbox{ship_source_rect.height * 2, ship_source_rect.width * 2, k_ship_width, k_ship_height});

    // Create stars
    for (int i = 0; i < engine_ctx.k_stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{(float)GetRandomValue(0, k_width),
                                                                      (float)GetRandomValue(0, k_height), 0, 0, 0, 0, 1,
                                                                      1, 1});
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }

    // Create enemies
    constexpr int k_spawn_margin = 100;
    constexpr float k_enemy_sprite_x = 5.0f;
    constexpr float k_enemy_sprite_y = 6.0f;
    constexpr float k_enemy_sprite_width = 21.0f;
    constexpr float k_enemy_sprite_height = 23.0f;
    constexpr float k_enemy_scale = 5.0f;
    constexpr float k_enemy_base_speed = 3.0f;
    constexpr float k_enemy_speed_variance = 5.0f;
    constexpr int k_enemy_health = 3;
    constexpr float k_enemy_hitbox_width = 15.0f;
    constexpr float k_enemy_hitbox_height = 18.0f;
    constexpr float k_pattern_base_speed = 201.0f;
    constexpr float k_pattern_speed_variance = 3.0f;
    constexpr int k_pattern_amplitude_max = 10;

    engine_ctx.assets_manager.load_texture("enemy_ship", "assets/sprites/r-typesheet5.gif");
    for (size_t i = 0; i < engine_ctx.k_max_enemies; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawn_y = (float)GetRandomValue(k_spawn_margin, k_height - k_spawn_margin);
        float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

        // Position
        engine_ctx.registry.add_component(enemy, engn::cpnt::Transform{spawn_x, spawn_y, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(
            enemy, cpnt::Velocity{-(k_enemy_base_speed + randf() * k_enemy_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, cpnt::Enemy{});
        engine_ctx.registry.add_component(
            enemy, cpnt::Sprite{{k_enemy_sprite_x, k_enemy_sprite_y, k_enemy_sprite_width, k_enemy_sprite_height},
                                k_enemy_scale,
                                0,
                                "enemy_ship"});
        engine_ctx.registry.add_component(enemy, cpnt::Health{k_enemy_health, k_enemy_health});

        // Create a **new MovementPattern instance** for this enemy
        cpnt::MovementPattern pat;
        pat.speed = k_pattern_base_speed + randf() * k_pattern_speed_variance;
        pat.amplitude = (float)GetRandomValue(1, k_pattern_amplitude_max);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        int pattern_nbr = GetRandomValue(0, 3);
        switch (pattern_nbr) {
            case 0:
                pat.type = cpnt::MovementPattern::PatternType::ZigZag;
                break;
            case 1:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
            case 2:
                pat.type = cpnt::MovementPattern::PatternType::Sine;
                break;
            case 3:
                pat.type = cpnt::MovementPattern::PatternType::Dive;
                break;
        }
        pat.base_y = spawn_y;

        engine_ctx.registry.add_component(enemy, std::move(pat));
        engine_ctx.registry.add_component(enemy, cpnt::Hitbox{k_enemy_hitbox_width, k_enemy_hitbox_height,
                                                              k_enemy_sprite_width, k_enemy_sprite_height});
    }

    // lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(),
    //     "scripts/lua/ui/game_menu.lua");
}
