#include "scenes_loaders.h"

#include <random>

#include "raylib.h"

#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "game_engine/systems/systems.h"
#include "game_engine/api/lua.h"
#include "systems/client_systems.h"

using namespace engn;

static float randf() {
    return (rand() % 1000) / 1000.0f;
}

void load_game_scene(engn::EngineContext& engine_ctx)
{
    const int k_width = engine_ctx.window_size.x;
    const int k_height = engine_ctx.window_size.y;

    auto &registry = engine_ctx.registry;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.1f, 0.8f);

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
    registry.register_component<cpnt::Weapon>();
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
    engine_ctx.add_system<>(sys::ui_press);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox>(sys::collision_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Explosion, cpnt::Sprite>(sys::explosion_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Particle, cpnt::Bullet>(sys::particle_emission_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(sys::player_control_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle>(sys::render_system);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_connection_menu_ui_events);

    // Create player
    Rectangle shipSourceRect = {166.0f, 0.0f, 33.0f, 18.0f};

    engine_ctx.assets_manager.load_texture("player_ship", "assets/sprites/r-typesheet1.gif");

    auto player = engine_ctx.registry.spawn_entity();
    engine_ctx.registry.add_component(player, cpnt::Transform{(float)k_width/2, (float)k_height/2, 0, 0, 0, 0, 0, 0, 1, 1, 1});
    engine_ctx.registry.add_component(player, cpnt::Player{});
    engine_ctx.registry.add_component(player, cpnt::Sprite{shipSourceRect, 3.0f, 0, "player_ship"});
    engine_ctx.registry.add_component(player, cpnt::Health{100, 100});
    engine_ctx.registry.add_component(player, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});

    // Create stars
    for (int i = 0; i < engine_ctx.stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{
            (float)GetRandomValue(0, k_width),
            (float)GetRandomValue(0, k_height),
            0, 0, 0, 0, 1, 1, 1
        });
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }

    // Create enemies
    engine_ctx.assets_manager.load_texture("enemy_ship", "assets/sprites/r-typesheet5.gif");
    for (size_t i = 0; i < engine_ctx.max_enemies; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawnY = (float)GetRandomValue(100, k_height - 100);
        float spawnX = (float)GetRandomValue(k_width, k_width * 2);

        // Position
        engine_ctx.registry.add_component(enemy, engn::cpnt::Transform{spawnX, spawnY, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(enemy, cpnt::Velocity{-(3.0f + randf() * 5.0f), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, cpnt::Enemy{});
        engine_ctx.registry.add_component(enemy, cpnt::Sprite{{5.0f, 6.0f, 21.0f, 23.0f}, 5.0f, 0, "enemy_ship"});
        engine_ctx.registry.add_component(enemy, cpnt::Health{3, 3});

        // Create a **new MovementPattern instance** for this enemy
        cpnt::MovementPattern pat;
        pat.speed = 201.f + randf() * 3.f;
        pat.amplitude = (float)GetRandomValue(1, 10);
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
        pat.baseY = spawnY;

        engine_ctx.registry.add_component(enemy, std::move(pat));
    }

    // lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(),
    //     "scripts/lua/ui/game_menu.lua");
}
