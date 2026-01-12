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
void handle_settings_menu_ui_events(engn::EngineContext& engine_ctx) {
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
            engine_ctx.pending_rebind = ControlAction::None;
            engine_ctx.confirm_keyboard_reset = false;
            engine_ctx.set_scene(engine_ctx.settings_return_scene);
            return;
        }
    }

    if (engine_ctx.pending_rebind != ControlAction::None) {
        if (key_evt) {
            switch (engine_ctx.pending_rebind) {
                case ControlAction::MoveUp:
                    engine_ctx.controls.move_up.primary = key_evt->keycode;
                    break;
                case ControlAction::MoveDown:
                    engine_ctx.controls.move_down.primary = key_evt->keycode;
                    break;
                case ControlAction::MoveLeft:
                    engine_ctx.controls.move_left.primary = key_evt->keycode;
                    break;
                case ControlAction::MoveRight:
                    engine_ctx.controls.move_right.primary = key_evt->keycode;
                    break;
                case ControlAction::Shoot:
                    engine_ctx.controls.shoot.primary = key_evt->keycode;
                    break;
                case ControlAction::None:
                    break;
            }
            engine_ctx.pending_rebind = ControlAction::None;
        }
    }

    const evts::MouseButtonReleased* mouse_evt =
        engine_ctx.input_event_queue.get_last<evts::MouseButtonReleased>();
    if (mouse_evt && mouse_evt->button == evts::MouseButton::MouseButtonLeft && !any_button_clicked) {
        engine_ctx.confirm_keyboard_reset = false;
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

    if (tag_name != "reset_controls_button") {
        ctx.confirm_keyboard_reset = false;
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
        ctx.pending_rebind = ControlAction::None;
        ctx.set_scene(k_settings_audio_scene_id);
        return true;
    } else if (tag_name == "reset_controls_button") {
        ctx.pending_rebind = ControlAction::None;
        if (!ctx.confirm_keyboard_reset) {
            ctx.confirm_keyboard_reset = true;
        } else {
            ctx.controls = make_default_controls();
            ctx.confirm_keyboard_reset = false;
        }
    } else if (tag_name == "rebind_move_up") {
        ctx.confirm_keyboard_reset = false;
        ctx.pending_rebind = (ctx.pending_rebind == ControlAction::MoveUp) ? ControlAction::None
                                                                           : ControlAction::MoveUp;
    } else if (tag_name == "rebind_move_down") {
        ctx.confirm_keyboard_reset = false;
        ctx.pending_rebind = (ctx.pending_rebind == ControlAction::MoveDown) ? ControlAction::None
                                                                             : ControlAction::MoveDown;
    } else if (tag_name == "rebind_move_left") {
        ctx.confirm_keyboard_reset = false;
        ctx.pending_rebind = (ctx.pending_rebind == ControlAction::MoveLeft) ? ControlAction::None
                                                                             : ControlAction::MoveLeft;
    } else if (tag_name == "rebind_move_right") {
        ctx.confirm_keyboard_reset = false;
        ctx.pending_rebind = (ctx.pending_rebind == ControlAction::MoveRight) ? ControlAction::None
                                                                              : ControlAction::MoveRight;
    } else if (tag_name == "rebind_shoot") {
        ctx.confirm_keyboard_reset = false;
        ctx.pending_rebind = (ctx.pending_rebind == ControlAction::Shoot) ? ControlAction::None
                                                                          : ControlAction::Shoot;
    }
    return false;
}

static const char* keycode_to_label(evts::KeyboardKeyCode keycode) {
    switch (keycode) {
        case evts::KeyboardKeyCode::KeyA: return "Q";
        case evts::KeyboardKeyCode::KeyB: return "B";
        case evts::KeyboardKeyCode::KeyC: return "C";
        case evts::KeyboardKeyCode::KeyD: return "D";
        case evts::KeyboardKeyCode::KeyE: return "E";
        case evts::KeyboardKeyCode::KeyF: return "F";
        case evts::KeyboardKeyCode::KeyG: return "G";
        case evts::KeyboardKeyCode::KeyH: return "H";
        case evts::KeyboardKeyCode::KeyI: return "I";
        case evts::KeyboardKeyCode::KeyJ: return "J";
        case evts::KeyboardKeyCode::KeyK: return "K";
        case evts::KeyboardKeyCode::KeyL: return "L";
        case evts::KeyboardKeyCode::KeyM: return "M";
        case evts::KeyboardKeyCode::KeyN: return "N";
        case evts::KeyboardKeyCode::KeyO: return "O";
        case evts::KeyboardKeyCode::KeyP: return "P";
        case evts::KeyboardKeyCode::KeyQ: return "A";
        case evts::KeyboardKeyCode::KeyR: return "R";
        case evts::KeyboardKeyCode::KeyS: return "S";
        case evts::KeyboardKeyCode::KeyT: return "T";
        case evts::KeyboardKeyCode::KeyU: return "U";
        case evts::KeyboardKeyCode::KeyV: return "V";
        case evts::KeyboardKeyCode::KeyW: return "Z";
        case evts::KeyboardKeyCode::KeyX: return "X";
        case evts::KeyboardKeyCode::KeyY: return "Y";
        case evts::KeyboardKeyCode::KeyZ: return "W";
        case evts::KeyboardKeyCode::Key0: return "0";
        case evts::KeyboardKeyCode::Key1: return "1";
        case evts::KeyboardKeyCode::Key2: return "2";
        case evts::KeyboardKeyCode::Key3: return "3";
        case evts::KeyboardKeyCode::Key4: return "4";
        case evts::KeyboardKeyCode::Key5: return "5";
        case evts::KeyboardKeyCode::Key6: return "6";
        case evts::KeyboardKeyCode::Key7: return "7";
        case evts::KeyboardKeyCode::Key8: return "8";
        case evts::KeyboardKeyCode::Key9: return "9";
        case evts::KeyboardKeyCode::KeyEscape: return "Esc";
        case evts::KeyboardKeyCode::KeyEnter: return "Enter";
        case evts::KeyboardKeyCode::KeyTab: return "Tab";
        case evts::KeyboardKeyCode::KeyBackspace: return "Backspace";
        case evts::KeyboardKeyCode::KeyInsert: return "Insert";
        case evts::KeyboardKeyCode::KeyDelete: return "Delete";
        case evts::KeyboardKeyCode::KeyPeriod: return ".";
        case evts::KeyboardKeyCode::KeyRight: return "Right";
        case evts::KeyboardKeyCode::KeyLeft: return "Left";
        case evts::KeyboardKeyCode::KeyDown: return "Down";
        case evts::KeyboardKeyCode::KeyUp: return "Up";
        case evts::KeyboardKeyCode::KeyPageUp: return "PageUp";
        case evts::KeyboardKeyCode::KeyPageDown: return "PageDown";
        case evts::KeyboardKeyCode::KeyHome: return "Home";
        case evts::KeyboardKeyCode::KeyEnd: return "End";
        case evts::KeyboardKeyCode::KeyCapsLock: return "CapsLock";
        case evts::KeyboardKeyCode::KeyScrollLock: return "ScrollLock";
        case evts::KeyboardKeyCode::KeyNumLock: return "NumLock";
        case evts::KeyboardKeyCode::KeyPrintScreen: return "PrintScreen";
        case evts::KeyboardKeyCode::KeyPause: return "Pause";
        case evts::KeyboardKeyCode::KeyF1: return "F1";
        case evts::KeyboardKeyCode::KeyF2: return "F2";
        case evts::KeyboardKeyCode::KeyF3: return "F3";
        case evts::KeyboardKeyCode::KeyF4: return "F4";
        case evts::KeyboardKeyCode::KeyF5: return "F5";
        case evts::KeyboardKeyCode::KeyF6: return "F6";
        case evts::KeyboardKeyCode::KeyF7: return "F7";
        case evts::KeyboardKeyCode::KeyF8: return "F8";
        case evts::KeyboardKeyCode::KeyF9: return "F9";
        case evts::KeyboardKeyCode::KeyF10: return "F10";
        case evts::KeyboardKeyCode::KeyF11: return "F11";
        case evts::KeyboardKeyCode::KeyF12: return "F12";
        case evts::KeyboardKeyCode::KeySpace: return "Space";
        case evts::KeyboardKeyCode::KeyLeftShift: return "LShift";
        case evts::KeyboardKeyCode::KeyRightShift: return "RShift";
        case evts::KeyboardKeyCode::KeyLeftControl: return "LCtrl";
        case evts::KeyboardKeyCode::KeyRightControl: return "RCtrl";
        case evts::KeyboardKeyCode::KeyLeftAlt: return "LAlt";
        case evts::KeyboardKeyCode::KeyRightAlt: return "RAlt";
        case evts::KeyboardKeyCode::KeyLeftSuper: return "LSuper";
        case evts::KeyboardKeyCode::KeyRightSuper: return "RSuper";
        case evts::KeyboardKeyCode::KeyUnknown: return "Unbound";
    }
    return "Unbound";
}

static std::string format_binding(const char* label, const ControlBinding& binding) {
    const bool k_has_primary = binding.primary != evts::KeyboardKeyCode::KeyUnknown;
    const bool k_has_secondary = binding.secondary != evts::KeyboardKeyCode::KeyUnknown;
    std::string text = std::string(label) + ": ";
    if (k_has_primary)
        text += keycode_to_label(binding.primary);
    if (k_has_secondary) {
        if (k_has_primary)
            text += " / ";
        text += keycode_to_label(binding.secondary);
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
    set_text_if_exists(ctx, "mapping_move_up", format_binding("Move Up", ctx.controls.move_up));
    set_text_if_exists(ctx, "mapping_move_down", format_binding("Move Down", ctx.controls.move_down));
    set_text_if_exists(ctx, "mapping_move_left", format_binding("Move Left", ctx.controls.move_left));
    set_text_if_exists(ctx, "mapping_move_right", format_binding("Move Right", ctx.controls.move_right));
    set_text_if_exists(ctx, "mapping_shoot", format_binding("Shoot", ctx.controls.shoot));
}

static const char* action_to_label(ControlAction action) {
    switch (action) {
        case ControlAction::MoveUp: return "Move Up";
        case ControlAction::MoveDown: return "Move Down";
        case ControlAction::MoveLeft: return "Move Left";
        case ControlAction::MoveRight: return "Move Right";
        case ControlAction::Shoot: return "Shoot";
        case ControlAction::None: return "";
    }
    return "";
}

static void update_prompt_text(EngineContext& ctx) {
    if (ctx.pending_rebind == ControlAction::None) {
        if (ctx.confirm_keyboard_reset) {
            set_text_if_exists(ctx, "rebind_prompt", "Press Reset again to confirm");
        } else {
            set_text_if_exists(ctx, "rebind_prompt", "Click Rebind to change controls");
        }
        return;
    }
    std::string prompt = "Press a key for ";
    prompt += action_to_label(ctx.pending_rebind);
    set_text_if_exists(ctx, "rebind_prompt", prompt);
}

static void update_rebind_buttons(EngineContext& ctx) {
    set_text_if_exists(ctx, "rebind_move_up",
                       ctx.pending_rebind == ControlAction::MoveUp ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_move_down",
                       ctx.pending_rebind == ControlAction::MoveDown ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_move_left",
                       ctx.pending_rebind == ControlAction::MoveLeft ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_move_right",
                       ctx.pending_rebind == ControlAction::MoveRight ? "Cancel" : "Rebind");
    set_text_if_exists(ctx, "rebind_shoot",
                       ctx.pending_rebind == ControlAction::Shoot ? "Cancel" : "Rebind");
}

static void update_reset_button(EngineContext& ctx) {
    set_text_if_exists(ctx, "reset_controls_button", ctx.confirm_keyboard_reset ? "Confirm" : "Reset");
}
