#pragma once

#include "events/controller_events.h"
#include "events/keyboard_events.h"

namespace engn {

struct InputState {
    float move_x = 0.0f;
    float move_y = 0.0f;
    bool shoot_pressed = false;
    bool shoot_held = false;
    bool using_gamepad = false;
};

enum class InputContext {
    Gameplay = 0,
    Menu
};

struct ControlBinding {
    evts::KeyboardKeyCode primary = evts::KeyboardKeyCode::KeyUnknown;
    evts::KeyboardKeyCode secondary = evts::KeyboardKeyCode::KeyUnknown;
};

struct ControlScheme {
    ControlBinding move_up;
    ControlBinding move_down;
    ControlBinding move_left;
    ControlBinding move_right;
    ControlBinding shoot;
};

struct GamepadControlBinding {
    evts::ControllerButton primary = evts::ControllerButton::ControllerButtonUnknown;
    evts::ControllerButton secondary = evts::ControllerButton::ControllerButtonUnknown;
};

struct GamepadControlScheme {
    GamepadControlBinding move_up;
    GamepadControlBinding move_down;
    GamepadControlBinding move_left;
    GamepadControlBinding move_right;
    GamepadControlBinding shoot;
};

enum class ControlAction {
    None = 0,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot
};

enum class GamepadControlAction {
    None = 0,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot
};

inline ControlScheme make_default_controls() {
    ControlScheme scheme;
    scheme.move_up = {evts::KeyboardKeyCode::KeyW, evts::KeyboardKeyCode::KeyUp};
    scheme.move_down = {evts::KeyboardKeyCode::KeyS, evts::KeyboardKeyCode::KeyDown};
    scheme.move_left = {evts::KeyboardKeyCode::KeyA, evts::KeyboardKeyCode::KeyLeft};
    scheme.move_right = {evts::KeyboardKeyCode::KeyD, evts::KeyboardKeyCode::KeyRight};
    scheme.shoot = {evts::KeyboardKeyCode::KeySpace, evts::KeyboardKeyCode::KeyUnknown};
    return scheme;
}

inline GamepadControlScheme make_default_gamepad_controls() {
    GamepadControlScheme scheme;
    scheme.move_up = {evts::ControllerButton::ControllerButtonDpadUp, evts::ControllerButton::ControllerButtonUnknown};
    scheme.move_down = {evts::ControllerButton::ControllerButtonDpadDown,
                        evts::ControllerButton::ControllerButtonUnknown};
    scheme.move_left = {evts::ControllerButton::ControllerButtonDpadLeft,
                        evts::ControllerButton::ControllerButtonUnknown};
    scheme.move_right = {evts::ControllerButton::ControllerButtonDpadRight,
                         evts::ControllerButton::ControllerButtonUnknown};
    scheme.shoot = {evts::ControllerButton::ControllerButtonSouth, evts::ControllerButton::ControllerButtonUnknown};
    return scheme;
}

} // namespace engn
