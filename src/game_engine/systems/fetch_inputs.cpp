#include "systems/systems.h"

#include <unordered_map>

#include "raylib.h"

#include "engine.h"
#include "events/events.h"

#include "utils/logger.h"

using namespace engn;

static void fetch_key_pressed_events(evts::EventQueue<evts::Event> &);
static void fetch_key_held_events(evts::EventQueue<evts::Event> &);
static void fetch_key_released_events(evts::EventQueue<evts::Event> &);

static void fetch_mouse_button_pressed_events(evts::EventQueue<evts::Event> &);
static void fetch_mouse_button_held_events(evts::EventQueue<evts::Event> &);
static void fetch_mouse_button_released_events(evts::EventQueue<evts::Event> &);
static void fetch_mouse_moved_events(evts::EventQueue<evts::Event> &);
static void fetch_mouse_scrolled_events(evts::EventQueue<evts::Event> &);

static void fetch_controller_button_pressed_events(evts::EventQueue<evts::Event> &, int);
static void fetch_controller_button_held_events(evts::EventQueue<evts::Event> &, int);
static void fetch_controller_button_released_events(evts::EventQueue<evts::Event> &, int);
static void fetch_controller_joystick_events(evts::EventQueue<evts::Event> &, int);
static void fetch_controller_trigger_events(evts::EventQueue<evts::Event> &, int);

const std::vector<std::string> k_controllers = {
    "Xbox",
    "DualShock",
    "DualSense"
};

void sys::fetch_inputs(EngineContext &ctx)
{
    auto &input_events = ctx.input_event_queue;
    int gamepad_id = -1;

    for (int i = 0; i < 25; i++) {
        if (!IsGamepadAvailable(i)) continue;
        if (gamepad_id != -1) break;

        const char* name = GetGamepadName(i);
        if (!name) continue;

        std::string n(name);
        for (const auto& cname : k_controllers) {
            if (n.find(cname) != std::string::npos) {
                gamepad_id = i;
                break;
            }
        }
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

const std::unordered_map<std::size_t, evts::KeyboardKeyCode> keyboard_lookup_table = {
    {KEY_A, evts::key_a},
    {KEY_B, evts::key_b},
    {KEY_C, evts::key_c},
    {KEY_D, evts::key_d},
    {KEY_E, evts::key_e},
    {KEY_F, evts::key_f},
    {KEY_G, evts::key_g},
    {KEY_H, evts::key_h},
    {KEY_I, evts::key_i},
    {KEY_J, evts::key_j},
    {KEY_K, evts::key_k},
    {KEY_L, evts::key_l},
    {KEY_M, evts::key_m},
    {KEY_N, evts::key_n},
    {KEY_O, evts::key_o},
    {KEY_P, evts::key_p},
    {KEY_Q, evts::key_q},
    {KEY_R, evts::key_r},
    {KEY_S, evts::key_s},
    {KEY_T, evts::key_t},
    {KEY_U, evts::key_u},
    {KEY_V, evts::key_v},
    {KEY_W, evts::key_w},
    {KEY_X, evts::key_x},
    {KEY_Y, evts::key_y},
    {KEY_Z, evts::key_z},

    {KEY_ONE, evts::key_1},
    {KEY_TWO, evts::key_2},
    {KEY_THREE, evts::key_3},
    {KEY_FOUR, evts::key_4},
    {KEY_FIVE, evts::key_5},
    {KEY_SIX, evts::key_6},
    {KEY_SEVEN, evts::key_7},
    {KEY_EIGHT, evts::key_8},
    {KEY_NINE, evts::key_9},
    {KEY_ZERO, evts::key_0},

    {KEY_ESCAPE, evts::key_escape},
    {KEY_ENTER, evts::key_enter},
    {KEY_TAB, evts::key_tab},
    {KEY_BACKSPACE, evts::key_backspace},
    {KEY_INSERT, evts::key_insert},
    {KEY_DELETE, evts::key_delete},

    {KEY_RIGHT, evts::key_right},
    {KEY_LEFT, evts::key_left},
    {KEY_DOWN, evts::key_down},
    {KEY_UP, evts::key_up},

    {KEY_PAGE_UP, evts::key_page_up},
    {KEY_PAGE_DOWN, evts::key_page_down},

    {KEY_HOME, evts::key_home},
    {KEY_END, evts::key_end},

    {KEY_CAPS_LOCK, evts::key_caps_lock},
    {KEY_SCROLL_LOCK, evts::key_scroll_lock},
    {KEY_NUM_LOCK, evts::key_num_lock},
    {KEY_PRINT_SCREEN, evts::key_print_screen},
    {KEY_PAUSE, evts::key_pause},

    {KEY_F1, evts::key_f1},
    {KEY_F2, evts::key_f2},
    {KEY_F3, evts::key_f3},
    {KEY_F4, evts::key_f4},
    {KEY_F5, evts::key_f5},
    {KEY_F6, evts::key_f6},
    {KEY_F7, evts::key_f7},
    {KEY_F8, evts::key_f8},
    {KEY_F9, evts::key_f9},
    {KEY_F10, evts::key_f10},
    {KEY_F11, evts::key_f11},
    {KEY_F12, evts::key_f12},

    {KEY_SPACE, evts::key_space},

    {KEY_LEFT_SHIFT, evts::key_left_shift},
    {KEY_RIGHT_SHIFT, evts::key_right_shift},

    {KEY_LEFT_ALT, evts::key_left_alt},
    {KEY_RIGHT_ALT, evts::key_right_alt},

    {KEY_LEFT_CONTROL, evts::key_left_control},
    {KEY_RIGHT_CONTROL, evts::key_right_control},

    {KEY_LEFT_SUPER, evts::key_left_super},
    {KEY_RIGHT_SUPER, evts::key_right_super}
};

static void fetch_key_pressed_events(evts::EventQueue<evts::Event> &input_events)
{
    for (const auto &[raylib_key, engn_key] : keyboard_lookup_table) {
        if (IsKeyPressed(raylib_key)) {
            evts::KeyPressed evt;

            evt.keycode = engn_key;
            input_events.push(evt);
        }
    }
}

static void fetch_key_held_events(evts::EventQueue<evts::Event> &input_events)
{
    for (const auto &[raylib_key, engn_key] : keyboard_lookup_table) {
        if (IsKeyDown(raylib_key)) {
            evts::KeyHold evt;

            evt.keycode = engn_key;
            input_events.push(evt);
        }
    }
}

static void fetch_key_released_events(evts::EventQueue<evts::Event> &input_events)
{
    for (const auto &[raylib_key, engn_key] : keyboard_lookup_table) {
        if (IsKeyReleased(raylib_key)) {
            evts::KeyReleased evt;

            evt.keycode = engn_key;
            input_events.push(evt);
        }
    }
}

const std::unordered_map<std::size_t, evts::MouseButton> mouse_button_lookup_table = {
    {MOUSE_BUTTON_LEFT, evts::mouse_button_left},
    {MOUSE_BUTTON_RIGHT, evts::mouse_button_right},
    {MOUSE_BUTTON_MIDDLE, evts::mouse_button_middle},
    {MOUSE_BUTTON_SIDE, evts::mouse_button_extra1},
    {MOUSE_BUTTON_EXTRA, evts::mouse_button_extra2}
};

static void fetch_mouse_button_pressed_events(evts::EventQueue<evts::Event> &input_events)
{
    for (const auto &[raylib_button, engn_button] : mouse_button_lookup_table) {
        if (IsMouseButtonPressed(raylib_button)) {
            evts::MouseButtonPressed evt;

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_mouse_button_held_events(evts::EventQueue<evts::Event> &input_events)
{
    for (const auto &[raylib_button, engn_button] : mouse_button_lookup_table) {
        if (IsMouseButtonDown(raylib_button)) {
            evts::MouseButtonHeld evt;

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_mouse_button_released_events(evts::EventQueue<evts::Event> &input_events)
{
    for (const auto &[raylib_button, engn_button] : mouse_button_lookup_table) {
        if (IsMouseButtonReleased(raylib_button)) {
            evts::MouseButtonReleased evt;

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_mouse_moved_events(evts::EventQueue<evts::Event> &input_events)
{
   Vector2 mouseDelta = GetMouseDelta();
   evts::MouseMoved evt;

   evt.x = GetMouseX();
   evt.y = GetMouseY();
   input_events.push(evt);
}

static void fetch_mouse_scrolled_events(evts::EventQueue<evts::Event> &input_events)
{
    Vector2 mouseWheelMove = GetMouseWheelMoveV();

    if (mouseWheelMove.x >= 0.1f || mouseWheelMove.x <= -0.1f ||
        mouseWheelMove.y >= 0.1f || mouseWheelMove.y <= -0.1f) {
        evts::MouseScrolled evt;

        evt.offset_x = mouseWheelMove.x;
        evt.offset_y = mouseWheelMove.y;
        input_events.push(evt);
    }
}

const std::unordered_map<std::size_t, evts::ControllerButton> controller_button_lookup_table = {
    {GAMEPAD_BUTTON_RIGHT_FACE_DOWN, evts::controller_button_south},
    {GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, evts::controller_button_east},
    {GAMEPAD_BUTTON_RIGHT_FACE_LEFT, evts::controller_button_west},
    {GAMEPAD_BUTTON_RIGHT_FACE_UP, evts::controller_button_north},

    {GAMEPAD_BUTTON_LEFT_TRIGGER_1, evts::controller_buttons_left_shoulder},
    {GAMEPAD_BUTTON_RIGHT_TRIGGER_1, evts::controller_buttons_right_shoulder},
    {GAMEPAD_BUTTON_LEFT_TRIGGER_2, evts::controller_buttons_left_trigger},
    {GAMEPAD_BUTTON_RIGHT_TRIGGER_2, evts::controller_buttons_right_trigger},

    {GAMEPAD_BUTTON_MIDDLE_LEFT, evts::controller_button_back},
    {GAMEPAD_BUTTON_MIDDLE_RIGHT, evts::controller_button_start},
    {GAMEPAD_BUTTON_MIDDLE, evts::controller_button_home},

    {GAMEPAD_BUTTON_LEFT_FACE_UP, evts::controller_button_dpad_up},
    {GAMEPAD_BUTTON_LEFT_FACE_DOWN, evts::controller_button_dpad_down},
    {GAMEPAD_BUTTON_LEFT_FACE_LEFT, evts::controller_button_dpad_left},
    {GAMEPAD_BUTTON_LEFT_FACE_RIGHT, evts::controller_button_dpad_right},

    {GAMEPAD_BUTTON_LEFT_THUMB, evts::controller_button_left_stick},
    {GAMEPAD_BUTTON_RIGHT_THUMB, evts::controller_button_right_stick}
};

static void fetch_controller_button_pressed_events(evts::EventQueue<evts::Event> &input_events, int gamepad_id)
{
    for (const auto &[raylib_button, engn_button] : controller_button_lookup_table) {
        if (IsGamepadButtonPressed(gamepad_id, raylib_button)) {
            evts::ControllerButtonPressed evt;

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_controller_button_held_events(evts::EventQueue<evts::Event> &input_events, int gamepad_id)
{
    for (const auto &[raylib_button, engn_button] : controller_button_lookup_table) {
        if (IsGamepadButtonDown(gamepad_id, raylib_button)) {
            evts::ControllerButtonHeld evt;

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_controller_button_released_events(evts::EventQueue<evts::Event> &input_events, int gamepad_id)
{
    for (const auto &[raylib_button, engn_button] : controller_button_lookup_table) {
        if (IsGamepadButtonReleased(gamepad_id, raylib_button)) {
            evts::ControllerButtonReleased evt;

            evt.button = engn_button;
            input_events.push(evt);
        }
    }
}

static void fetch_controller_joystick_events(evts::EventQueue<evts::Event> &input_events, int gamepad_id)
{
    float leftStickX = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_X);
    float leftStickY = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_Y);
    Vector2 leftStick = {leftStickX, leftStickY};
    float rightStickX = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_X);
    float rightStickY = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_Y);
    Vector2 rightStick = {rightStickX, rightStickY};

    if (leftStick.x >= 0.1f || leftStick.x <= -0.1f ||
        leftStick.y >= 0.1f || leftStick.y <= -0.1f) {
        evts::ControllerLeftJoystickMove evt;

        evt.x = leftStick.x;
        evt.y = leftStick.y;
        input_events.push(evt);
    }

    if (rightStick.x >= 0.1f || rightStick.x <= -0.1f ||
        rightStick.y >= 0.1f || rightStick.y <= -0.1f) {
        evts::ControllerRightJoystickMove evt;

        evt.x = rightStick.x;
        evt.y = rightStick.y;
        input_events.push(evt);
    }
}

static void fetch_controller_trigger_events(evts::EventQueue<evts::Event> &input_events, int gamepad_id)
{
    float leftTrigger = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_LEFT_TRIGGER);
    float rightTrigger = GetGamepadAxisMovement(gamepad_id, GAMEPAD_AXIS_RIGHT_TRIGGER);

    if (leftTrigger >= 0.1f || leftTrigger <= -0.1f) {
        evts::ControllerLeftTriggerMove evt;

        evt.value = leftTrigger;
        input_events.push(evt);
    }

    if (rightTrigger >= 0.1f || rightTrigger <= -0.1f) {
        evts::ControllerRightTriggerMove evt;

        evt.value = rightTrigger;
        input_events.push(evt);
    }
}
