#pragma once

namespace engn {

namespace evts {

enum MouseButton {
    mouse_button_unknown = -1,

    mouse_button_left = 0,
    mouse_button_right,
    mouse_button_middle,
    mouse_button_extra1, // typically the "back" button
    mouse_button_extra2 // typically the "forward" button
};

struct MouseButtonPressed {
    MouseButton button;
};

struct MouseButtonHeld {
    MouseButton button;
};

struct MouseButtonReleased {
    MouseButton button;
};

struct MouseMoved {
    int x;
    int y;
};

struct MouseScrolled {
    float offset_x;
    float offset_y;
};

} // namespace events

} // namespace engine