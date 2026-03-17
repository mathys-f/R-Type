#include "game_engine/api/lua.h"
#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "scenes_loaders.h"
#include "systems/client_systems.h"

#include <iomanip>
#include <sstream>
#include <string>

#include "sol/sol.hpp"

using namespace engn;

namespace {
const std::string k_lost_script_file = "scripts/lua/ui/game_over_lost.lua";
const std::string k_won_script_file = "scripts/lua/ui/game_over_won.lua";
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
constexpr int k_score_padding = 6;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor; // NOLINT(clang-analyzer-security.insecureAPI.rand)
}

static void set_text_if_exists(EngineContext& ctx, const char* tag, const std::string& value) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value()) {
        return;
    }

    auto& texts = ctx.registry.get_components<cpnt::UIText>();
    auto& text = texts[ent_opt.value()];
    if (!text.has_value()) {
        return;
    }

    text->content = value;
}

void load_game_over_scene(engn::EngineContext& engine_ctx) {
    auto& reg = engine_ctx.registry;
    engine_ctx.input_context = InputContext::Menu;
    const bool k_has_boss_target = engine_ctx.game_over_boss_kills_to_win > 0;
    const bool k_won_game = k_has_boss_target && engine_ctx.game_over_stats.boss_killed >= engine_ctx.game_over_boss_kills_to_win;

    reg.register_component<cpnt::Transform>();
    reg.register_component<cpnt::Sprite>();
    reg.register_component<cpnt::Velocity>();
    reg.register_component<cpnt::Particle>();
    reg.register_component<cpnt::Boss>();
    reg.register_component<cpnt::UIButton>();
    reg.register_component<cpnt::UIFocusable>();
    reg.register_component<cpnt::UIInputField>();
    reg.register_component<cpnt::UIInteractable>();
    reg.register_component<cpnt::UINavigation>();
    reg.register_component<cpnt::UISlider>();
    reg.register_component<cpnt::UIStyle>();
    reg.register_component<cpnt::UIText>();
    reg.register_component<cpnt::UITransform>();

    reg.register_component<cpnt::Star>();
    reg.register_component<cpnt::Stats>();

    engn::lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(), k_won_game ? k_won_script_file : k_lost_script_file);

    engine_ctx.add_system<>(sys::fetch_inputs);
    // engine_ctx.add_system<>(sys::log_inputs);
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_game_over_ui_events);
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle, cpnt::Stats, cpnt::Boss>(
        sys::render_system);

    std::ostringstream score_stream;
    score_stream << std::setw(k_score_padding) << std::setfill('0') << engine_ctx.game_over_stats.score;
    set_text_if_exists(engine_ctx, "score_value", score_stream.str());
    set_text_if_exists(engine_ctx, "dmg_value", std::to_string(engine_ctx.game_over_stats.dmg));
    set_text_if_exists(engine_ctx, "kills_value", std::to_string(engine_ctx.game_over_stats.kills));
    set_text_if_exists(engine_ctx, "level_value", std::to_string(engine_ctx.game_over_stats.level));

    std::ostringstream boss_stream;
    if (k_has_boss_target) {
        boss_stream << engine_ctx.game_over_stats.boss_killed << " / " << engine_ctx.game_over_boss_kills_to_win;
    } else {
        boss_stream << engine_ctx.game_over_stats.boss_killed << " / 0";
    }
    set_text_if_exists(engine_ctx, "boss_killed_value", boss_stream.str());

    set_text_if_exists(engine_ctx, "continue_hint", k_won_game ? "Victory condition reached" : "Run ended before boss objective");

    const int k_width = static_cast<int>(engine_ctx.window_size.x); // NOLINT(cppcoreguidelines-pro-type-union-access)
    const int k_height =
        static_cast<int>(engine_ctx.window_size.y); // NOLINT(cppcoreguidelines-pro-type-union-access)

    for (int i = 0; i < engine_ctx.k_stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{(float)GetRandomValue(0, k_width),
                                                                      (float)GetRandomValue(0, k_height), 0, 0, 0, 0, 1,
                                                                      1, 1});
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }
}
