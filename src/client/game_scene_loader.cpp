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
    registry.register_component<cpnt::Bullet_shooter>();
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
    registry.register_component<cpnt::Boss>();
    registry.register_component<cpnt::BossHitbox>();
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
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet_shooter>(sys::bullet_shooter_system);

    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox,
        cpnt::Bullet_shooter, cpnt::Shooter, cpnt::Stats, cpnt::BossHitbox>(
        sys::collision_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Explosion, cpnt::Sprite>(sys::explosion_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Particle, cpnt::Bullet, cpnt::Bullet_shooter>(sys::particle_emission_system);
    engine_ctx.add_system<>(sys::resolve_player_input);
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(
        sys::player_control_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle, cpnt::Stats, cpnt::Boss>(
        sys::render_system);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_game_pause_inputs);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity, cpnt::Shooter, cpnt::Player>(sys::shooter_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Health, cpnt::Sprite, cpnt::Shooter, cpnt::Player>(sys::shooter_system);
    engine_ctx.add_system<cpnt::Stats>(sys::stat_system);
    engine_ctx.add_system<cpnt::Boss, cpnt::Stats, cpnt::BossHitbox>(sys::boss_system);

    // Load assets

    engine_ctx.assets_manager.load_texture("bulletExplosion", "assets/sprites/r-typesheet43.gif");
    engine_ctx.assets_manager.load_texture("explosion", "assets/sprites/r-typesheet44.gif");
    engine_ctx.assets_manager.load_texture("boss", "assets/sprites/r-typesheet30.gif");

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
        player, cpnt::Hitbox{ k_ship_width  * k_ship_scale / 2, k_ship_height * k_ship_scale / 2, ship_source_rect.height / 3, ship_source_rect.width / 3});

    // Create Stats

    auto stats = engine_ctx.registry.spawn_entity();

    engine_ctx.registry.add_component(stats, cpnt::Stats{0, 0, 0, 0});

    // Create stars
    for (int i = 0; i < engine_ctx.k_stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{(float)GetRandomValue(0, k_width),
                                                                      (float)GetRandomValue(0, k_height), 0, 0, 0, 0, 1,
                                                                      1, 1});
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }

    // Create enemies
    constexpr float k_enemy_sprite_x = 5.0f;
    constexpr float k_enemy_sprite_y = 6.0f;
    constexpr float k_enemy_sprite_width = 21.0f;
    constexpr float k_enemy_sprite_height = 23.0f;
    constexpr float k_enemy_scale = 5.0f;
    constexpr float k_enemy_hitbox_width = 15.0f;
    constexpr float k_enemy_hitbox_height = 18.0f;
    constexpr float k_pattern_base_speed = 201.0f;

    engine_ctx.assets_manager.load_texture("enemy_ship", "assets/sprites/r-typesheet5.gif");
    for (size_t i = 0; i < 0; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawn_y = (float)GetRandomValue(engine_ctx.k_spawn_margin, k_height - engine_ctx.k_spawn_margin);
        float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

        // Position
        engine_ctx.registry.add_component(enemy, engn::cpnt::Transform{spawn_x, spawn_y, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(
            enemy, cpnt::Velocity{-(engine_ctx.k_enemy_base_speed + randf() * engine_ctx.k_enemy_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, cpnt::Enemy{});
        engine_ctx.registry.add_component(
            enemy, cpnt::Sprite{{k_enemy_sprite_x, k_enemy_sprite_y, k_enemy_sprite_width, k_enemy_sprite_height},
                                k_enemy_scale,
                                0,
                                "enemy_ship"});
        engine_ctx.registry.add_component(enemy, cpnt::Health{engine_ctx.k_enemy_health, engine_ctx.k_enemy_health});

        // Create a **new MovementPattern instance** for this enemy
        cpnt::MovementPattern pat;
        pat.speed = k_pattern_base_speed + randf() * engine_ctx.k_pattern_speed_variance;
        pat.amplitude = (float)GetRandomValue(1, engine_ctx.k_pattern_amplitude_max);
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
        engine_ctx.registry.add_component(enemy, cpnt::Hitbox{k_enemy_hitbox_width * k_enemy_scale, k_enemy_hitbox_height * k_enemy_scale,
                                                              k_enemy_sprite_width, k_enemy_sprite_height});
    }

    // Create shooters

    constexpr float k_shooter_sprite_x = 87.0f;
    constexpr float k_shooter_sprite_y = 67.0f;
    constexpr float k_shooter_sprite_width = 22.0f;
    constexpr float k_shooter_sprite_height = 18.0f;
    constexpr float k_shooter_scale = 5.0f;
    constexpr float k_shooter_hitbox_width = 15.0f;
    constexpr float k_shooter_hitbox_height = 18.0f;

    engine_ctx.assets_manager.load_texture("shooter_sprite", "assets/sprites/r-typesheet19.gif");
    engine_ctx.assets_manager.load_texture("shooter_bullet", "assets/sprites/r-typesheet1_bis.gif");

    for (size_t i = 0; i < 0; i++) {
        auto shooter = engine_ctx.registry.spawn_entity();

        float spawn_y = (float)GetRandomValue(engine_ctx.k_spawn_margin, k_height - engine_ctx.k_spawn_margin);
        float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

        // Position
        engine_ctx.registry.add_component(shooter, engn::cpnt::Transform{spawn_x, spawn_y, 0, 55.f, 45.f, 0, 1, 1, 1}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)

        // Velocity
        engine_ctx.registry.add_component(
            shooter, cpnt::Velocity{-(engine_ctx.k_shooter_base_speed + randf() * engine_ctx.k_shooter_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(shooter, cpnt::Shooter{0});
        engine_ctx.registry.add_component(
            shooter, cpnt::Sprite{{k_shooter_sprite_x, k_shooter_sprite_y, k_shooter_sprite_width, k_shooter_sprite_height},
                                k_shooter_scale,
                                0,
                                "shooter_sprite"});
        engine_ctx.registry.add_component(shooter, cpnt::Health{engine_ctx.k_shooter_health, engine_ctx.k_shooter_health});

        // Create a **new MovementPattern instance** for this shooter
        cpnt::MovementPattern pat;
        pat.speed = k_pattern_base_speed + randf() * engine_ctx.k_pattern_speed_variance;
        pat.amplitude = (float)GetRandomValue(1, engine_ctx.k_pattern_amplitude_max);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        int pattern_nbr = GetRandomValue(0, 3);
        switch (pattern_nbr) {
            case 0:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
            case 1:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
            case 2:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
            case 3:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
        }
        pat.base_y = spawn_y;

        engine_ctx.registry.add_component(shooter, std::move(pat));
        engine_ctx.registry.add_component(shooter, cpnt::Hitbox{k_shooter_hitbox_width * (k_shooter_scale + 2), k_shooter_hitbox_height * (k_shooter_scale + 2),
                                                              -k_shooter_sprite_width * 2, -k_shooter_sprite_height * 3});
    }



    // Spawn Boss

    /*
top right: 27 861

bottom left: 181 1064

width: 154

height: 203
*/
    const float boss_sprite_x = 27.0f;
    const float boss_sprite_y = 861.0f;
    const float boss_sprite_width = 154.0f;
    const float boss_sprite_height = 203.0f;
    const float boss_scale = 5.0f;
    const float boss_hitbox_head_width = 111.f * boss_scale;
    const float boss_hitbox_head_height = 86.f * boss_scale;
    const float boss_hitbox_body_width = 76.f * boss_scale;
    const float boss_hitbox_body_height = 93.f * boss_scale;
    const float boss_hitbox_tail_width = 68.f * boss_scale;
    const float boss_hitbox_tail_height = 30.f * boss_scale;
    const float boss_hitbox_head_x_offset = 0.f;
    const float boss_hitbox_head_y_offset = 0.f;
    const float boss_hitbox_body_x_offset = 83.f * boss_scale;
    const float boss_hitbox_body_y_offset = 85.f * boss_scale;
    const float boss_hitbox_tail_x_offset = 55.f * boss_scale;
    const float boss_hitbox_tail_y_offset = 176.f * boss_scale;


    auto boss = engine_ctx.registry.spawn_entity();
    engine_ctx.registry.add_component(boss, cpnt::Transform{1200.f, 100.f, 0, 0, 0, 0, 1, 1, 1});
    engine_ctx.registry.add_component(boss, cpnt::Boss{0.f, false, {1350.f, 400.f}, 0.f, 600.f});
    engine_ctx.registry.add_component(
        boss, cpnt::Sprite{{27.0f, 861.0f, 154.0f, 203.0f}, boss_scale, 0, "boss"});
    engine_ctx.registry.add_component(boss, cpnt::Health{100, 100});
    engine_ctx.registry.add_component(boss, cpnt::Velocity{0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
    engine_ctx.registry.add_component(boss, cpnt::BossHitbox{
        boss_hitbox_head_width, boss_hitbox_head_height, boss_hitbox_head_x_offset, boss_hitbox_head_y_offset,
        boss_hitbox_body_width, boss_hitbox_body_height, boss_hitbox_body_x_offset, boss_hitbox_body_y_offset,
        boss_hitbox_tail_width, boss_hitbox_tail_height, boss_hitbox_tail_x_offset, boss_hitbox_tail_y_offset
    });

    // lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(),
    //     "scripts/lua/ui/game_menu.lua");
}

//        27.f, 861.f, 110.f, 85.f,
//        105.f, 947.f, 76.f, 93.f,
//        82.f, 1040.f, 68.f, 24.f