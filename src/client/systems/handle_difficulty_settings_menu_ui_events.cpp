#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "raylib.h"
#include "systems/client_systems.h"

#include <iomanip>
#include <sstream>
#include <string>

using namespace engn;

namespace {
constexpr int k_max_shooter_min = 1;
constexpr int k_max_shooter_max = 20;
constexpr int k_max_shooter_step = 1;

constexpr int k_enemy_health_min = 1;
constexpr int k_enemy_health_max = 20;
constexpr int k_enemy_health_step = 1;

constexpr float k_enemy_speed_min = 0.5f;
constexpr float k_enemy_speed_max = 10.0f;
constexpr float k_enemy_speed_step = 0.5f;

constexpr int k_player_health_min = 10;
constexpr int k_player_health_max = 500;
constexpr int k_player_health_step = 10;
} // namespace

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt);
static void update_difficulty_text(EngineContext& ctx);
static void set_text_if_exists(EngineContext& ctx, const char* tag, const std::string& value);
static void apply_preset_easy(EngineContext& ctx);
static void apply_preset_normal(EngineContext& ctx);
static void apply_preset_hard(EngineContext& ctx);

void handle_difficulty_settings_menu_ui_events(engn::EngineContext& engine_ctx) {
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
        engine_ctx.set_scene("main_menu");
        return;
    }

    update_difficulty_text(engine_ctx);
}

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(evt.tag);

    if (tag_name == "back_button") {
        ctx.set_scene("main_menu");
        return true;
    } else if (tag_name == "max_shooter_down_button") {
        if (ctx.k_max_shooter > k_max_shooter_min) {
            ctx.k_max_shooter -= k_max_shooter_step;
            if (ctx.k_max_charger > k_max_shooter_min)
                ctx.k_max_charger -= k_max_shooter_step;
        }
    } else if (tag_name == "max_shooter_up_button") {
        if (ctx.k_max_shooter < k_max_shooter_max) {
            ctx.k_max_shooter += k_max_shooter_step;
            if (ctx.k_max_charger < k_max_shooter_max)
                ctx.k_max_charger += k_max_shooter_step;
        }
    } else if (tag_name == "enemy_health_down_button") {
        if (ctx.k_enemy_health > k_enemy_health_min)
            ctx.k_enemy_health -= k_enemy_health_step;
    } else if (tag_name == "enemy_health_up_button") {
        if (ctx.k_enemy_health < k_enemy_health_max)
            ctx.k_enemy_health += k_enemy_health_step;
    } else if (tag_name == "enemy_speed_down_button") {
        if (ctx.k_enemy_base_speed > k_enemy_speed_min)
            ctx.k_enemy_base_speed -= k_enemy_speed_step;
    } else if (tag_name == "enemy_speed_up_button") {
        if (ctx.k_enemy_base_speed < k_enemy_speed_max)
            ctx.k_enemy_base_speed += k_enemy_speed_step;
    } else if (tag_name == "player_health_down_button") {
        if (ctx.k_player_health > k_player_health_min)
            ctx.k_player_health -= k_player_health_step;
    } else if (tag_name == "player_health_up_button") {
        if (ctx.k_player_health < k_player_health_max)
            ctx.k_player_health += k_player_health_step;
    } else if (tag_name == "preset_easy_button") {
        apply_preset_easy(ctx);
    } else if (tag_name == "preset_normal_button") {
        apply_preset_normal(ctx);
    } else if (tag_name == "preset_hard_button") {
        apply_preset_hard(ctx);
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

static void update_difficulty_text(EngineContext& ctx) {
    // Clamp values to valid ranges
    if (ctx.k_max_shooter < k_max_shooter_min)
        ctx.k_max_shooter = k_max_shooter_min;
    if (ctx.k_max_shooter > k_max_shooter_max)
        ctx.k_max_shooter = k_max_shooter_max;
    
    if (ctx.k_enemy_health < k_enemy_health_min)
        ctx.k_enemy_health = k_enemy_health_min;
    if (ctx.k_enemy_health > k_enemy_health_max)
        ctx.k_enemy_health = k_enemy_health_max;
    
    if (ctx.k_enemy_base_speed < k_enemy_speed_min)
        ctx.k_enemy_base_speed = k_enemy_speed_min;
    if (ctx.k_enemy_base_speed > k_enemy_speed_max)
        ctx.k_enemy_base_speed = k_enemy_speed_max;
    
    if (ctx.k_player_health < k_player_health_min)
        ctx.k_player_health = k_player_health_min;
    if (ctx.k_player_health > k_player_health_max)
        ctx.k_player_health = k_player_health_max;

    // Update UI text
    set_text_if_exists(ctx, "max_shooter_value", std::to_string(ctx.k_max_shooter));
    set_text_if_exists(ctx, "enemy_health_value", std::to_string(ctx.k_enemy_health));
    set_text_if_exists(ctx, "player_health_value", std::to_string(ctx.k_player_health));
    
    // Format float to 1 decimal place
    std::ostringstream speed_stream;
    speed_stream << std::fixed << std::setprecision(1) << ctx.k_enemy_base_speed;
    set_text_if_exists(ctx, "enemy_speed_value", speed_stream.str());
}

static void apply_preset_easy(EngineContext& ctx) {
    ctx.k_max_bullets = 150; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_max_charger = 3; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_max_shooter = 2; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_base_speed = 2.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_speed_variance = 3.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_health = 2; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_base_speed = 2.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_speed_variance = 3.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_health = 2; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_player_health = 150; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
}

static void apply_preset_normal(EngineContext& ctx) {
    ctx.k_max_bullets = 100; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_max_charger = 5; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_max_shooter = 5; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_base_speed = 3.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_speed_variance = 5.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_health = 3; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_base_speed = 3.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_speed_variance = 5.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_health = 3; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_player_health = 100; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
}

static void apply_preset_hard(EngineContext& ctx) {
    ctx.k_max_bullets = 75; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_max_charger = 8; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_max_shooter = 10; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_base_speed = 5.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_speed_variance = 7.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_shooter_health = 5; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_base_speed = 5.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_speed_variance = 7.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_enemy_health = 5; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    ctx.k_player_health = 50; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
}
