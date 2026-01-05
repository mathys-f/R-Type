#pragma once

namespace engn {

namespace evts {

enum class ControllerButton {
    ControllerButtonUnknown = 0,

    ControllerButtonSouth, // A on Xbox, Cross on PlayStation, B on Nintendo
    ControllerButtonEast,  // B on Xbox, Circle on PlayStation, A on Nintendo
    ControllerButtonWest,  // X on Xbox, Square on PlayStation, Y on Nintendo
    ControllerButtonNorth, // Y on Xbox, Triangle on PlayStation, X on Nintendo

    ControllerButtonLeftShoulder,  // LB on Xbox, L1 on PlayStation
    ControllerButtonRightShoulder, // RB on Xbox, R1 on PlayStation
    ControllerButtonLeftTrigger,   // LT on Xbox, L2 on PlayStation
    ControllerButtonRightTrigger,  // RT on Xbox, R2 on PlayStation

    ControllerButtonBack,  // Back button on Xbox, Screenshot button on PlayStation
    ControllerButtonStart, // Start button on Xbox, Menu button on PlayStation
    ControllerButtonHome,  // Xbox button on Xbox, PS button on PlayStation

    ControllerButtonDpadUp,
    ControllerButtonDpadDown,
    ControllerButtonDpadLeft,
    ControllerButtonDpadRight,

    ControllerButtonLeftStick,  // Pressing down the left stick
    ControllerButtonRightStick, // Pressing down the right stick
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

} // namespace engn