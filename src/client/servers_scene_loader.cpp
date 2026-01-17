#include "game_engine/api/lua.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "raylib.h"
#include "scenes_loaders.h"
#include "systems/client_systems.h"

#include <string>

#include "sol/sol.hpp"

using namespace engn;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor; // NOLINT(clang-analyzer-security.insecureAPI.rand)
}

const std::string k_script_file = "scripts/lua/ui/servers_menu.lua";

void load_servers_scene(engn::EngineContext& engine_ctx) {
    auto& reg = engine_ctx.registry;
    reset_servers_ui_state();
    engine_ctx.input_context = InputContext::Menu;

    reg.register_component<cpnt::UIButton>();
    reg.register_component<cpnt::UIFocusable>();
    reg.register_component<cpnt::UIInputField>();
    reg.register_component<cpnt::UIInteractable>();
    reg.register_component<cpnt::UINavigation>();
    reg.register_component<cpnt::UIScrollArea>();
    reg.register_component<cpnt::UISlider>();
    reg.register_component<cpnt::UIStyle>();
    reg.register_component<cpnt::UIText>();
    reg.register_component<cpnt::UITransform>();

    reg.register_component<cpnt::Star>();
    reg.register_component<cpnt::Transform>();
    reg.register_component<cpnt::Sprite>();
    reg.register_component<cpnt::Velocity>();
    reg.register_component<cpnt::Particle>();

    engine_ctx.add_system<>(sys::fetch_inputs);
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    engine_ctx.add_system<cpnt::UIInteractable>(sys::ui_input_field_updater);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIScrollArea>(sys::ui_scroll_area);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle, cpnt::Stats,
                          cpnt::Boss>(sys::render_system);
    engine_ctx.add_system<>(handle_servers_ui_events);

    engn::lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(), k_script_file);

    const int k_width = static_cast<int>(engine_ctx.window_size.x); // NOLINT(cppcoreguidelines-pro-type-union-access)
    const int k_height = static_cast<int>(engine_ctx.window_size.y); // NOLINT(cppcoreguidelines-pro-type-union-access)

    for (int i = 0; i < engine_ctx.k_stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{(float)GetRandomValue(0, k_width),
                                                                      (float)GetRandomValue(0, k_height), 0, 0, 0, 0, 1,
                                                                      1, 1});
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }
}
