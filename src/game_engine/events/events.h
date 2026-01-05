#pragma once

#include "controller_events.h"
#include "keyboard_events.h"
#include "mouse_events.h"

#include <variant>

namespace engn {

namespace evts {

using Event =
    std::variant<KeyPressed, KeyHold, KeyReleased,

                 MouseButtonPressed, MouseButtonHeld, MouseButtonReleased, MouseMoved, MouseScrolled,

                 ControllerButtonPressed, ControllerButtonHeld, ControllerButtonReleased, ControllerLeftJoystickMove,
                 ControllerRightJoystickMove, ControllerLeftTriggerMove, ControllerRightTriggerMove

                 >;
} // namespace evts

} // namespace engn
