#pragma once

namespace engn {

namespace evts {

enum ControllerButton {
    controller_button_unknown = 0,

    controller_button_south, // A on Xbox, Cross on PlayStation, B on Nintendo
    controller_button_east,  // B on Xbox, Circle on PlayStation, A on Nintendo
    controller_button_west,  // X on Xbox, Square on PlayStation, Y on Nintendo
    controller_button_north, // Y on Xbox, Triangle on PlayStation, X on Nintendo

    controller_buttons_left_shoulder,  // LB on Xbox, L1 on PlayStation
    controller_buttons_right_shoulder, // RB on Xbox, R1 on PlayStation
    controller_buttons_left_trigger,   // LT on Xbox, L2 on PlayStation
    controller_buttons_right_trigger,  // RT on Xbox, R2 on PlayStation

    controller_button_back,  // Back button on Xbox, Screenshot button on PlayStation
    controller_button_start, // Start button on Xbox, Menu button on PlayStation
    controller_button_home,  // Xbox button on Xbox, PS button on PlayStation

    controller_button_dpad_up,
    controller_button_dpad_down,
    controller_button_dpad_left,
    controller_button_dpad_right,

    controller_button_left_stick,  // Pressing down the left stick
    controller_button_right_stick, // Pressing down the right stick
};

struct ControllerButtonPressed {
    ControllerButton button;
};

struct ControllerButtonHeld {
    ControllerButton button;
};

struct ControllerButtonReleased {
    ControllerButton button;
};

struct ControllerLeftJoystickMove {
    float x; // -1.0 (left) to 1.0 (right)
    float y; // -1.0 (down) to 1.0 (up)
};

struct ControllerRightJoystickMove {
    float x; // -1.0 (left) to 1.0 (right)
    float y; // -1.0 (down) to 1.0 (up)
};

struct ControllerLeftTriggerMove {
    float value; // -1.0 (not pressed) to 1.0 (fully pressed), 0 is the treshold for being considered pressed
};

struct ControllerRightTriggerMove {
    float value; // -1.0 (not pressed) to 1.0 (fully pressed), 0 is the treshold for being considered pressed
};

} // namespace evts

} // namespace engine