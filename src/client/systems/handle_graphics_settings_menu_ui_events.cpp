#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "raylib.h"
#include "systems/client_systems.h"

#include <string>

using namespace engn;

namespace {
constexpr int k_scroll_speed_min = 1;
constexpr int k_scroll_speed_max = 20;
constexpr int k_scroll_speed_step = 1;

constexpr int k_particles_min = 1;
constexpr int k_particles_max = 10;
constexpr int k_particles_step = 1;

constexpr int k_stars_min = 100;
constexpr int k_stars_max = 5000;
constexpr int k_stars_step = 100;
} // namespace

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt);
static void update_graphics_text(EngineContext& ctx);
static void set_text_if_exists(EngineContext& ctx, const char* tag, const std::string& value);

void handle_graphics_settings_menu_ui_events(engn::EngineContext& engine_ctx) {
    const auto& evts = engine_ctx.ui_event_queue;

    bool switched_scene = false;
    evts.for_each<evts::UIButtonClicked>([&engine_ctx, &switched_scene](const evts::UIButtonClicked& evt) {
        if (handle_ui_button_clicked(engine_ctx, evt))
            switched_scene = true;
    });
    if (switched_scene)
        return;

    const evts::KeyPressed* key_evt = engine_ctx.input_event_queue.get_last<evts::KeyPressed>();
    const evts::ControllerButtonPressed* pad_evt =
        engine_ctx.input_event_queue.get_last<evts::ControllerButtonPressed>();
    const bool k_escape_pressed = key_evt && key_evt->keycode == evts::KeyboardKeyCode::KeyEscape;
    const bool k_pause_pressed =
        pad_evt && (pad_evt->button == evts::ControllerButton::ControllerButtonStart ||
                    pad_evt->button == evts::ControllerButton::ControllerButtonBack);
    if (k_escape_pressed || k_pause_pressed) {
        if (engine_ctx.settings_return_scene != "") {
            engine_ctx.set_scene(engine_ctx.settings_return_scene);
            return;
        }
    }

    update_graphics_text(engine_ctx);
}

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(evt.tag);

    if (tag_name == "back_button") {
        ctx.set_scene(ctx.settings_return_scene);
        return true;
    } else if (tag_name == "exit_button") {
        ctx.should_quit = true;
        return true;
    } else if (tag_name == "main_menu_button") {
        ctx.settings_return_scene = "";
        ctx.set_scene("main_menu");
        return true;
    } else if (tag_name == "nav_controls_button") {
        ctx.set_scene("keyboard_settings");
        return true;
    } else if (tag_name == "nav_gamepad_button") {
        ctx.set_scene("gamepad_settings");
        return true;
    } else if (tag_name == "nav_audio_button") {
        ctx.set_scene("audio_settings");
        return true;
    } else if (tag_name == "nav_graphics_button") {
        // Already on graphics settings, do nothing
    } else if (tag_name == "scroll_speed_down_button") {
        if (ctx.k_scroll_speed > k_scroll_speed_min)
            ctx.k_scroll_speed -= k_scroll_speed_step;
    } else if (tag_name == "scroll_speed_up_button") {
        if (ctx.k_scroll_speed < k_scroll_speed_max)
            ctx.k_scroll_speed += k_scroll_speed_step;
    } else if (tag_name == "particles_down_button") {
        if (ctx.k_particles > k_particles_min)
            ctx.k_particles -= k_particles_step;
    } else if (tag_name == "particles_up_button") {
        if (ctx.k_particles < k_particles_max)
            ctx.k_particles += k_particles_step;
    } else if (tag_name == "stars_down_button") {
        if (ctx.k_stars > k_stars_min)
            ctx.k_stars -= k_stars_step;
    } else if (tag_name == "stars_up_button") {
        if (ctx.k_stars < k_stars_max)
            ctx.k_stars += k_stars_step;
    }
    return false;
}

static void set_text_if_exists(EngineContext& ctx, const char* tag, const std::string& value) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value())
        return;
    auto& texts = ctx.registry.get_components<cpnt::UIText>();
    auto& text = texts[ent_opt.value()];
    if (!text.has_value())
        return;
    text->content = value;
}

static void update_graphics_text(EngineContext& ctx) {
    // Clamp values to valid ranges
    if (ctx.k_scroll_speed < k_scroll_speed_min)
        ctx.k_scroll_speed = k_scroll_speed_min;
    if (ctx.k_scroll_speed > k_scroll_speed_max)
        ctx.k_scroll_speed = k_scroll_speed_max;
    
    if (ctx.k_particles < k_particles_min)
        ctx.k_particles = k_particles_min;
    if (ctx.k_particles > k_particles_max)
        ctx.k_particles = k_particles_max;
    
    if (ctx.k_stars < k_stars_min)
        ctx.k_stars = k_stars_min;
    if (ctx.k_stars > k_stars_max)
        ctx.k_stars = k_stars_max;

    // Update UI text
    set_text_if_exists(ctx, "scroll_speed_value", std::to_string(ctx.k_scroll_speed));
    set_text_if_exists(ctx, "particles_value", std::to_string(ctx.k_particles));
    set_text_if_exists(ctx, "stars_value", std::to_string(ctx.k_stars));
}
