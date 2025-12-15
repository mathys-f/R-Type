#pragma once

#include <variant>

#include "keyboard_events.h"
#include "mouse_events.h"
#include "controller_events.h"

namespace engn {

namespace evts {

using Event = std::variant<
    KeyPressed,
    KeyHold,
    KeyReleased,

    MouseButtonPressed,
    MouseButtonHeld,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,

    ControllerButtonPressed,
    ControllerButtonHeld,
    ControllerButtonReleased,
    ControllerLeftJoystickMove,
    ControllerRightJoystickMove,
    ControllerLeftTriggerMove,
    ControllerRightTriggerMove

>;
} // namespace events

} // namespace engine
