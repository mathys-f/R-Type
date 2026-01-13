#include "engine.h"
#include "events/events.h"
#include "raylib.h"
#include "systems/systems.h"
#include "utils/logger.h"

#include <unordered_map>

using namespace engn;

namespace {
constexpr int k_max_gamepad_id = 4;
constexpr float k_input_threshold = 0.1f;
} // namespace

static void fetch_key_pressed_events(evts::EventQueue<evts::Event>&);
static void fetch_key_held_events(evts::EventQueue<evts::Event>&);
static void fetch_key_released_events(evts::EventQueue<evts::Event>&);

static void fetch_mouse_button_pressed_events(evts::EventQueue<evts::Event>&);
static void fetch_mouse_button_held_events(evts::EventQueue<evts::Event>&);
static void fetch_mouse_button_released_events(evts::EventQueue<evts::Event>&);
static void fetch_mouse_moved_events(evts::EventQueue<evts::Event>&);
static void fetch_mouse_scrolled_events(evts::EventQueue<evts::Event>&);

static void fetch_controller_button_pressed_events(evts::EventQueue<evts::Event>&, int);
static void fetch_controller_button_held_events(evts::EventQueue<evts::Event>&, int);
static void fetch_controller_button_released_events(evts::EventQueue<evts::Event>&, int);
static void fetch_controller_joystick_events(evts::EventQueue<evts::Event>&, int);
static void fetch_controller_trigger_events(evts::EventQueue<evts::Event>&, int);

const std::vector<std::string> k_controllers = {"Xbox", "DualShock", "DualSense"};

static int s_previous_gamepad_id = -1;

void sys::fetch_inputs(EngineContext& ctx) {
    auto& input_events = ctx.input_event_queue;
    int gamepad_id = -1;

    for (int i = 0; i < k_max_gamepad_id; i++) {
        if (!IsGamepadAvailable(i))
            continue;
        if (gamepad_id != -1)
            break;

        const char* name = GetGamepadName(i);
        if (!name)
            continue;

        std::string n(name);
        for (const auto& cname : k_controllers) {
            if (n.find(cname) != std::string::npos) {
                gamepad_id = i;
                break;
            }
        }
    }

    if (gamepad_id != s_previous_gamepad_id) {
        if (gamepad_id != -1) {
            const char* name = GetGamepadName(gamepad_id);
            if (name) {
                LOG_INFO("Gamepad detected: '{}' (id {})", name, gamepad_id);
            } else {
                LOG_INFO("Gamepad detected with id {}", gamepad_id);
            }
        } else if (s_previous_gamepad_id != -1) {
            LOG_INFO("Gamepad disconnected (id {})", s_previous_gamepad_id);
        }
        s_previous_gamepad_id = gamepad_id;
    }

    input_events.clear();

    fetch_key_pressed_events(input_events);
    fetch_key_held_events(input_events);
    fetch_key_released_events(input_events);

    fetch_mouse_button_pressed_events(input_events);
    fetch_mouse_button_held_events(input_events);
    fetch_mouse_button_released_events(input_events);
    fetch_mouse_moved_events(input_events);
    fetch_mouse_scrolled_events(input_events);

    if (gamepad_id != -1) {
        fetch_controller_button_pressed_events(input_events, gamepad_id);
        fetch_controller_button_held_events(input_events, gamepad_id);
        fetch_controller_button_released_events(input_events, gamepad_id);
        fetch_controller_joystick_events(input_events, gamepad_id);
        fetch_controller_trigger_events(input_events, gamepad_id);
    }
}

const std::unordered_map<std::size_t, evts::KeyboardKeyCode> k_keyboard_lookup_table = {
    {KEY_A, evts::KeyboardKeyCode::KeyA},
    {KEY_B, evts::KeyboardKeyCode::KeyB},
    {KEY_C, evts::KeyboardKeyCode::KeyC},
    {KEY_D, evts::KeyboardKeyCode::KeyD},
    {KEY_E, evts::KeyboardKeyCode::KeyE},
    {KEY_F, evts::KeyboardKeyCode::KeyF},
    {KEY_G, evts::KeyboardKeyCode::KeyG},
    {KEY_H, evts::KeyboardKeyCode::KeyH},
    {KEY_I, evts::KeyboardKeyCode::KeyI},
    {KEY_J, evts::KeyboardKeyCode::KeyJ},
    {KEY_K, evts::KeyboardKeyCode::KeyK},
    {KEY_L, evts::KeyboardKeyCode::KeyL},
    {KEY_M, evts::KeyboardKeyCode::KeyM},
    {KEY_N, evts::KeyboardKeyCode::KeyN},
    {KEY_O, evts::KeyboardKeyCode::KeyO},
    {KEY_P, evts::KeyboardKeyCode::KeyP},
    {KEY_Q, evts::KeyboardKeyCode::KeyQ},
    {KEY_R, evts::KeyboardKeyCode::KeyR},
    {KEY_S, evts::KeyboardKeyCode::KeyS},
    {KEY_T, evts::KeyboardKeyCode::KeyT},
    {KEY_U, evts::KeyboardKeyCode::KeyU},
    {KEY_V, evts::KeyboardKeyCode::KeyV},
    {KEY_W, evts::KeyboardKeyCode::KeyW},
    {KEY_X, evts::KeyboardKeyCode::KeyX},
    {KEY_Y, evts::KeyboardKeyCode::KeyY},
    {KEY_Z, evts::KeyboardKeyCode::KeyZ},

    {KEY_ONE, evts::KeyboardKeyCode::Key1},
    {KEY_TWO, evts::KeyboardKeyCode::Key2},
    {KEY_THREE, evts::KeyboardKeyCode::Key3},
    {KEY_FOUR, evts::KeyboardKeyCode::Key4},
    {KEY_FIVE, evts::KeyboardKeyCode::Key5},
    {KEY_SIX, evts::KeyboardKeyCode::Key6},
    {KEY_SEVEN, evts::KeyboardKeyCode::Key7},
    {KEY_EIGHT, evts::KeyboardKeyCode::Key8},
    {KEY_NINE, evts::KeyboardKeyCode::Key9},
    {KEY_ZERO, evts::KeyboardKeyCode::Key0},

    {KEY_ESCAPE, evts::KeyboardKeyCode::KeyEscape},
    {KEY_ENTER, evts::KeyboardKeyCode::KeyEnter},
    {KEY_TAB, evts::KeyboardKeyCode::KeyTab},
    {KEY_BACKSPACE, evts::KeyboardKeyCode::KeyBackspace},
    {KEY_INSERT, evts::KeyboardKeyCode::KeyInsert},
    {KEY_DELETE, evts::KeyboardKeyCode::KeyDelete},

    {KEY_PERIOD, evts::KeyboardKeyCode::KeyPeriod},

    {KEY_RIGHT, evts::KeyboardKeyCode::KeyRight},
    {KEY_LEFT, evts::KeyboardKeyCode::KeyLeft},
    {KEY_DOWN, evts::KeyboardKeyCode::KeyDown},
    {KEY_UP, evts::KeyboardKeyCode::KeyUp},

    {KEY_PAGE_UP, evts::KeyboardKeyCode::KeyPageUp},
    {KEY_PAGE_DOWN, evts::KeyboardKeyCode::KeyPageDown},

    {KEY_HOME, evts::KeyboardKeyCode::KeyHome},
    {KEY_END, evts::KeyboardKeyCode::KeyEnd},

    {KEY_CAPS_LOCK, evts::KeyboardKeyCode::KeyCapsLock},
    {KEY_SCROLL_LOCK, evts::KeyboardKeyCode::KeyScrollLock},
    {KEY_NUM_LOCK, evts::KeyboardKeyCode::KeyNumLock},
    {KEY_PRINT_SCREEN, evts::KeyboardKeyCode::KeyPrintScreen},
    {KEY_PAUSE, evts::KeyboardKeyCode::KeyPause},

    {KEY_F1, evts::KeyboardKeyCode::KeyF1},
    {KEY_F2, evts::KeyboardKeyCode::KeyF2},
    {KEY_F3, evts::KeyboardKeyCode::KeyF3},
    {KEY_F4, evts::KeyboardKeyCode::KeyF4},
    {KEY_F5, evts::KeyboardKeyCode::KeyF5},
    {KEY_F6, evts::KeyboardKeyCode::KeyF6},
    {KEY_F7, evts::KeyboardKeyCode::KeyF7},
    {KEY_F8, evts::KeyboardKeyCode::KeyF8},
    {KEY_F9, evts::KeyboardKeyCode::KeyF9},
    {KEY_F10, evts::KeyboardKeyCode::KeyF10},
    {KEY_F11, evts::KeyboardKeyCode::KeyF11},
    {KEY_F12, evts::KeyboardKeyCode::KeyF12},

    {KEY_SPACE, evts::KeyboardKeyCode::KeySpace},

    {KEY_LEFT_SHIFT, evts::KeyboardKeyCode::KeyLeftShift},
    {KEY_RIGHT_SHIFT, evts::KeyboardKeyCode::KeyRightShift},

    {KEY_LEFT_ALT, evts::KeyboardKeyCode::KeyLeftAlt},
    {KEY_RIGHT_ALT, evts::KeyboardKeyCode::KeyRightAlt},

    {KEY_LEFT_CONTROL, evts::KeyboardKeyCode::KeyLeftControl},
    {KEY_RIGHT_CONTROL, evts::KeyboardKeyCode::KeyRightControl},

    {KEY_LEFT_SUPER, evts::KeyboardKeyCode::KeyLeftSuper},
    {KEY_RIGHT_SUPER, evts::KeyboardKeyCode::KeyRightSuper}};

static void fetch_key_pressed_events(evts::EventQueue<evts::Event>& input_events) {
    for (const auto& [raylib_key, engn_key] : k_keyboard_lookup_table) {
        if (IsKeyPressed(static_cast<int>(raylib_key))) {
            evts::KeyPressed evt{};

            evt.keycode = engn_key;
            input_events.push(evt);
        }
    }
}

static void fetch_key_held_events(evts::EventQueue<evts::Event>& input_events) {
    for (const auto& [raylib_key, engn_key] : k_keyboard_lookup_table) {
        if (IsKeyDown(static_cast<int>(raylib_key))) {
            evts::KeyHold evt{};

            evt.keycode = engn_key;
            input_events.push(evt);
        }
    }
}

static void fetch_key_released_events(evts::EventQueue<evts::Event>& input_events) {
    for (const auto& [raylib_key, engn_key] : k_keyboard_lookup_table) {
        if (IsKeyReleased(static_cast<int>(raylib_key))) {
            evts::KeyReleased evt{};

            evt.keycode = engn_key;
            input_events.push(evt);
        }
    }
}

const std::unordered_map<std::size_t, evts::MouseButton> k_mouse_button_lookup_table = {
    {MOUSE_BUTTON_LEFT, evts::MouseButton::MouseButtonLeft},
    {MOUSE_BUTTON_RIGHT, evts::MouseButton::MouseButtonRight},
    {MOUSE_BUTTON_MIDDLE, evts::MouseButton::MouseButtonMiddle},
    {MOUSE_BUTTON_SIDE, evts::MouseButton::MouseButtonExtra1},
    {MOUSE_BUTTON_EXTRA, evts::MouseButton::MouseButtonExtra2}};

static void fetch_mouse_button_pressed_events(evts::EventQueue<evts::Event>& input_events) {
    for (const auto& [raylib_button, engn_button] : k_mouse_button_lookup_table) {
        if (IsMouseButtonPressed(static_cast<int>(raylib_button))) {
            evts::MouseButtonPressed evt{};

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_mouse_button_held_events(evts::EventQueue<evts::Event>& input_events) {
    for (const auto& [raylib_button, engn_button] : k_mouse_button_lookup_table) {
        if (IsMouseButtonDown(static_cast<int>(raylib_button))) {
            evts::MouseButtonHeld evt{};

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_mouse_button_released_events(evts::EventQueue<evts::Event>& input_events) {
    for (const auto& [raylib_button, engn_button] : k_mouse_button_lookup_table) {
        if (IsMouseButtonReleased(static_cast<int>(raylib_button))) {
            evts::MouseButtonReleased evt{};

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_mouse_moved_events(evts::EventQueue<evts::Event>& input_events) {
    evts::MouseMoved evt{};

    evt.x = static_cast<float>(GetMouseX());
    evt.y = static_cast<float>(GetMouseY());
    input_events.push(evt);
}

static void fetch_mouse_scrolled_events(evts::EventQueue<evts::Event>& input_events) {
    Vector2 mouse_wheel_move = GetMouseWheelMoveV();

    if (mouse_wheel_move.x >= k_input_threshold || mouse_wheel_move.x <= -k_input_threshold ||
        mouse_wheel_move.y >= k_input_threshold || mouse_wheel_move.y <= -k_input_threshold) {
        evts::MouseScrolled evt{};

        evt.offset_x = mouse_wheel_move.x;
        evt.offset_y = mouse_wheel_move.y;
        input_events.push(evt);
    }
}

const std::unordered_map<std::size_t, evts::ControllerButton> k_controller_button_lookup_table = {
    {GAMEPAD_BUTTON_RIGHT_FACE_DOWN, evts::ControllerButton::ControllerButtonSouth},
    {GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, evts::ControllerButton::ControllerButtonEast},
    {GAMEPAD_BUTTON_RIGHT_FACE_LEFT, evts::ControllerButton::ControllerButtonWest},
    {GAMEPAD_BUTTON_RIGHT_FACE_UP, evts::ControllerButton::ControllerButtonNorth},

    {GAMEPAD_BUTTON_LEFT_TRIGGER_1, evts::ControllerButton::ControllerButtonLeftShoulder},
    {GAMEPAD_BUTTON_RIGHT_TRIGGER_1, evts::ControllerButton::ControllerButtonRightShoulder},
    {GAMEPAD_BUTTON_LEFT_TRIGGER_2, evts::ControllerButton::ControllerButtonLeftTrigger},
    {GAMEPAD_BUTTON_RIGHT_TRIGGER_2, evts::ControllerButton::ControllerButtonRightTrigger},

    {GAMEPAD_BUTTON_MIDDLE_LEFT, evts::ControllerButton::ControllerButtonBack},
    {GAMEPAD_BUTTON_MIDDLE_RIGHT, evts::ControllerButton::ControllerButtonStart},
    {GAMEPAD_BUTTON_MIDDLE, evts::ControllerButton::ControllerButtonHome},

    {GAMEPAD_BUTTON_LEFT_FACE_UP, evts::ControllerButton::ControllerButtonDpadUp},
    {GAMEPAD_BUTTON_LEFT_FACE_DOWN, evts::ControllerButton::ControllerButtonDpadDown},
    {GAMEPAD_BUTTON_LEFT_FACE_LEFT, evts::ControllerButton::ControllerButtonDpadLeft},
    {GAMEPAD_BUTTON_LEFT_FACE_RIGHT, evts::ControllerButton::ControllerButtonDpadRight},

    {GAMEPAD_BUTTON_LEFT_THUMB, evts::ControllerButton::ControllerButtonLeftStick},
    {GAMEPAD_BUTTON_RIGHT_THUMB, evts::ControllerButton::ControllerButtonRightStick}};

static void fetch_controller_button_pressed_events(evts::EventQueue<evts::Event>& input_events, int gamepad_id) {
    for (const auto& [raylib_button, engn_button] : k_controller_button_lookup_table) {
        if (IsGamepadButtonPressed(gamepad_id, static_cast<int>(raylib_button))) {
            evts::ControllerButtonPressed evt{};

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_controller_button_held_events(evts::EventQueue<evts::Event>& input_events, int gamepad_id) {
    for (const auto& [raylib_button, engn_button] : k_controller_button_lookup_table) {
        if (IsGamepadButtonDown(gamepad_id, static_cast<int>(raylib_button))) {
            evts::ControllerButtonHeld evt{};

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_controller_button_released_events(evts::EventQueue<evts::Event>& input_events, int gamepad_id) {
    for (const auto& [raylib_button, engn_button] : k_controller_button_lookup_table) {
        if (IsGamepadButtonReleased(gamepad_id, static_cast<int>(raylib_button))) {
            evts::ControllerButtonReleased evt{};

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_controller_joystick_events(evts::EventQueue<evts::Event>& input_events, int gamepad_id) {
    float left_stick_x = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_X);
    float left_stick_y = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_Y);
    Vector2 left_stick = {left_stick_x, left_stick_y};
    float right_stick_x = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_X);
    float right_stick_y = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_Y);
    Vector2 right_stick = {right_stick_x, right_stick_y};

    if (left_stick.x >= k_input_threshold || left_stick.x <= -k_input_threshold || left_stick.y >= k_input_threshold ||
        left_stick.y <= -k_input_threshold) {
        evts::ControllerLeftJoystickMove evt{};

        evt.x = left_stick.x;
        evt.y = left_stick.y;
        input_events.push(evt);
    }

    if (right_stick.x >= k_input_threshold || right_stick.x <= -k_input_threshold ||
        right_stick.y >= k_input_threshold || right_stick.y <= -k_input_threshold) {
        evts::ControllerRightJoystickMove evt{};

        evt.x = right_stick.x;
        evt.y = right_stick.y;
        input_events.push(evt);
    }
}

static void fetch_controller_trigger_events(evts::EventQueue<evts::Event>& input_events, int gamepad_id) {
    float left_trigger = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_TRIGGER);
    float right_trigger = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_TRIGGER);

    if (left_trigger >= k_input_threshold || left_trigger <= -k_input_threshold) {
        evts::ControllerLeftTriggerMove evt{};

        evt.value = left_trigger;
        input_events.push(evt);
    }

    if (right_trigger >= k_input_threshold || right_trigger <= -k_input_threshold) {
        evts::ControllerRightTriggerMove evt{};

        evt.value = right_trigger;
        input_events.push(evt);
    }
}
