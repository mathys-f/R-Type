#pragma once

#include "events/keyboard_events.h"

namespace engn {

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

enum class ControlAction {
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

} // namespace engn
