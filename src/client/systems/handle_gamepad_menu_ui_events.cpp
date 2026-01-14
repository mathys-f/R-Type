#include "game_engine/components/components.h"
#include "game_engine/controls.h"
#include "game_engine/engine.h"
#include "systems/client_systems.h"

#include <string>

using namespace engn;

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt);
static void sync_settings_texts(EngineContext& ctx);
static void update_prompt_text(EngineContext& ctx);
static void update_rebind_buttons(EngineContext& ctx);
static void update_reset_button(EngineContext& ctx);
void handle_gamepad_menu_ui_events(engn::EngineContext& engine_ctx) {
    const auto& evts = engine_ctx.ui_event_queue;

    bool switched_scene = false;
    bool any_button_clicked = false;
    evts.for_each<evts::UIButtonClicked>([&engine_ctx, &switched_scene, &any_button_clicked](const evts::UIButtonClicked& evt) {
        any_button_clicked = true;
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
        if (engine_ctx.settings_return_scene != 1) {
            engine_ctx.pending_gamepad_rebind = GamepadControlAction::None;
            engine_ctx.confirm_gamepad_reset = false;
            engine_ctx.skip_next_gamepad_rebind_input = false;
            engine_ctx.set_scene(engine_ctx.settings_return_scene);
            return;
        }
    }

    if (engine_ctx.pending_gamepad_rebind != GamepadControlAction::None) {
        if (pad_evt) {
            if (engine_ctx.skip_next_gamepad_rebind_input) {
                engine_ctx.skip_next_gamepad_rebind_input = false;
                return;
            }
            if (pad_evt->button == evts::ControllerButton::ControllerButtonSouth) {
                engine_ctx.pending_gamepad_rebind = GamepadControlAction::None;
                return;
            }
            switch (engine_ctx.pending_gamepad_rebind) {
                case GamepadControlAction::MoveUp:
                    engine_ctx.gamepad_controls.move_up.primary = pad_evt->button;
                    break;
                case GamepadControlAction::MoveDown:
                    engine_ctx.gamepad_controls.move_down.primary = pad_evt->button;
                    break;
                case GamepadControlAction::MoveLeft:
                    engine_ctx.gamepad_controls.move_left.primary = pad_evt->button;
                    break;
                case GamepadControlAction::MoveRight:
                    engine_ctx.gamepad_controls.move_right.primary = pad_evt->button;
                    break;
                case GamepadControlAction::Shoot:
                    engine_ctx.gamepad_controls.shoot.primary = pad_evt->button;
                    break;
                case GamepadControlAction::None:
                    break;
            }
            engine_ctx.pending_gamepad_rebind = GamepadControlAction::None;
        }
    }

    const evts::MouseButtonReleased* mouse_evt =
        engine_ctx.input_event_queue.get_last<evts::MouseButtonReleased>();
    if (mouse_evt && mouse_evt->button == evts::MouseButton::MouseButtonLeft && !any_button_clicked) {
        engine_ctx.confirm_gamepad_reset = false;
    }

    sync_settings_texts(engine_ctx);
    update_prompt_text(engine_ctx);
    update_rebind_buttons(engine_ctx);
    update_reset_button(engine_ctx);
}

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(evt.tag);
    constexpr unsigned char k_settings_audio_scene_id = 5;
    constexpr unsigned char k_settings_controls_scene_id = 4;
    constexpr unsigned char k_settings_gamepad_scene_id = 6;

    if (tag_name != "reset_gamepad_button") {
        ctx.confirm_gamepad_reset = false;
    }

    if (tag_name == "back_button") {
        ctx.set_scene(ctx.settings_return_scene);
        return true;
    } else if (tag_name == "exit_button") {
        ctx.should_quit = true;
        return true;
    } else if (tag_name == "main_menu_button") {
        ctx.settings_return_scene = 1;
        ctx.set_scene(1);
        return true;
    } else if (tag_name == "nav_controls_button") {
        ctx.set_scene(k_settings_controls_scene_id);
        return true;
    } else if (tag_name == "nav_gamepad_button") {
        ctx.set_scene(k_settings_gamepad_scene_id);
        return true;
    } else if (tag_name == "nav_audio_button") {
        ctx.set_scene(k_settings_audio_scene_id);
        return true;
    } else if (tag_name == "reset_gamepad_button") {
        ctx.pending_gamepad_rebind = GamepadControlAction::None;
        ctx.skip_next_gamepad_rebind_input = false;
        if (!ctx.confirm_gamepad_reset) {
            ctx.confirm_gamepad_reset = true;
        } else {
            ctx.gamepad_controls = make_default_gamepad_controls();
            ctx.confirm_gamepad_reset = false;
        }
    } else if (tag_name == "rebind_gamepad_move_up") {
        ctx.confirm_gamepad_reset = false;
        ctx.skip_next_gamepad_rebind_input = true;
        ctx.pending_gamepad_rebind = (ctx.pending_gamepad_rebind == GamepadControlAction::MoveUp)
                                         ? GamepadControlAction::None
                                         : GamepadControlAction::MoveUp;
    } else if (tag_name == "rebind_gamepad_move_down") {
        ctx.confirm_gamepad_reset = false;
        ctx.skip_next_gamepad_rebind_input = true;
        ctx.pending_gamepad_rebind = (ctx.pending_gamepad_rebind == GamepadControlAction::MoveDown)
                                         ? GamepadControlAction::None
                                         : GamepadControlAction::MoveDown;
    } else if (tag_name == "rebind_gamepad_move_left") {
        ctx.confirm_gamepad_reset = false;
        ctx.skip_next_gamepad_rebind_input = true;
        ctx.pending_gamepad_rebind = (ctx.pending_gamepad_rebind == GamepadControlAction::MoveLeft)
                                         ? GamepadControlAction::None
                                         : GamepadControlAction::MoveLeft;
    } else if (tag_name == "rebind_gamepad_move_right") {
        ctx.confirm_gamepad_reset = false;
        ctx.skip_next_gamepad_rebind_input = true;
        ctx.pending_gamepad_rebind = (ctx.pending_gamepad_rebind == GamepadControlAction::MoveRight)
                                         ? GamepadControlAction::None
                                         : GamepadControlAction::MoveRight;
    } else if (tag_name == "rebind_gamepad_shoot") {
        ctx.confirm_gamepad_reset = false;
        ctx.skip_next_gamepad_rebind_input = true;
        ctx.pending_gamepad_rebind = (ctx.pending_gamepad_rebind == GamepadControlAction::Shoot)
                                         ? GamepadControlAction::None
                                         : GamepadControlAction::Shoot;
    }
    return false;
}

static const char* button_to_label(evts::ControllerButton button) {
    switch (button) {
        case evts::ControllerButton::ControllerButtonSouth: return "South";
        case evts::ControllerButton::ControllerButtonEast: return "East";
        case evts::ControllerButton::ControllerButtonWest: return "West";
        case evts::ControllerButton::ControllerButtonNorth: return "North";
        case evts::ControllerButton::ControllerButtonLeftShoulder: return "LB";
        case evts::ControllerButton::ControllerButtonRightShoulder: return "RB";
        case evts::ControllerButton::ControllerButtonLeftTrigger: return "LT";
        case evts::ControllerButton::ControllerButtonRightTrigger: return "RT";
        case evts::ControllerButton::ControllerButtonBack: return "Back";
        case evts::ControllerButton::ControllerButtonStart: return "Start";
        case evts::ControllerButton::ControllerButtonHome: return "Home";
        case evts::ControllerButton::ControllerButtonDpadUp: return "Dpad Up";
        case evts::ControllerButton::ControllerButtonDpadDown: return "Dpad Down";
        case evts::ControllerButton::ControllerButtonDpadLeft: return "Dpad Left";
        case evts::ControllerButton::ControllerButtonDpadRight: return "Dpad Right";
        case evts::ControllerButton::ControllerButtonLeftStick: return "L3";
        case evts::ControllerButton::ControllerButtonRightStick: return "R3";
        case evts::ControllerButton::ControllerButtonUnknown: return "Unbound";
    }
    return "Unbound";
}

static std::string format_binding(const char* label, const GamepadControlBinding& binding) {
    const bool k_has_primary = binding.primary != evts::ControllerButton::ControllerButtonUnknown;
    const bool k_has_secondary = binding.secondary != evts::ControllerButton::ControllerButtonUnknown;
    std::string text = std::string(label) + ": ";
    if (k_has_primary)
        text += button_to_label(binding.primary);
    if (k_has_secondary) {
        if (k_has_primary)
            text += " / ";
        text += button_to_label(binding.secondary);
    }
    if (!k_has_primary && !k_has_secondary)
        text += "Unbound";
    return text;
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

static void sync_settings_texts(EngineContext& ctx) {
    set_text_if_exists(ctx, "gamepad_mapping_move_up", format_binding("Move Up", ctx.gamepad_controls.move_up));
    set_text_if_exists(ctx, "gamepad_mapping_move_down", format_binding("Move Down", ctx.gamepad_controls.move_down));
    set_text_if_exists(ctx, "gamepad_mapping_move_left", format_binding("Move Left", ctx.gamepad_controls.move_left));
    set_text_if_exists(ctx, "gamepad_mapping_move_right", format_binding("Move Right", ctx.gamepad_controls.move_right));
    set_text_if_exists(ctx, "gamepad_mapping_shoot", format_binding("Shoot", ctx.gamepad_controls.shoot));
}

static const char* action_to_label(GamepadControlAction action) {
    switch (action) {
        case GamepadControlAction::MoveUp: return "Move Up";
        case GamepadControlAction::MoveDown: return "Move Down";
        case GamepadControlAction::MoveLeft: return "Move Left";
        case GamepadControlAction::MoveRight: return "Move Right";
        case GamepadControlAction::Shoot: return "Shoot";
        case GamepadControlAction::None: return "";
    }
    return "";
}

static void update_prompt_text(EngineContext& ctx) {
    if (ctx.pending_gamepad_rebind == GamepadControlAction::None) {
        if (ctx.confirm_gamepad_reset) {
            set_text_if_exists(ctx, "gamepad_rebind_prompt", "Press Reset again to confirm");
        } else {
            set_text_if_exists(ctx, "gamepad_rebind_prompt", "Click Rebind to change controls");
        }
        return;
    }
    std::string prompt = "Press a button for ";
    prompt += action_to_label(ctx.pending_gamepad_rebind);
    set_text_if_exists(ctx, "gamepad_rebind_prompt", prompt);
}

static void update_rebind_buttons(EngineContext& ctx) {
    set_text_if_exists(ctx, "rebind_gamepad_move_up",
                       ctx.pending_gamepad_rebind == GamepadControlAction::MoveUp ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_gamepad_move_down",
                       ctx.pending_gamepad_rebind == GamepadControlAction::MoveDown ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_gamepad_move_left",
                       ctx.pending_gamepad_rebind == GamepadControlAction::MoveLeft ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_gamepad_move_right",
                       ctx.pending_gamepad_rebind == GamepadControlAction::MoveRight ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_gamepad_shoot",
                       ctx.pending_gamepad_rebind == GamepadControlAction::Shoot ? "Cancel" : "Rebind");
}

static void update_reset_button(EngineContext& ctx) {
    set_text_if_exists(ctx, "reset_gamepad_button", ctx.confirm_gamepad_reset ? "Confirm" : "Reset");
}
