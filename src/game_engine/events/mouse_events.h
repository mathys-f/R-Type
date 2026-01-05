#pragma once

namespace engn {

namespace evts {

enum class MouseButton {
    MouseButtonUnknown = 0,

    MouseButtonLeft,
    MouseButtonRight,
    MouseButtonMiddle,
    MouseButtonExtra1, // typically the "back" button
    MouseButtonExtra2  // typically the "forward" button
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
    float x;
    float y;
};

struct MouseScrolled {
    float offset_x;
    float offset_y;
};

} // namespace evts

} // namespace engn