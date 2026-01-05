#include "engine.h"
#include "systems/systems.h"
#include "utils/logger.h"

#include <variant>

using namespace engn;

void sys::log_inputs(EngineContext& ctx) {
    for (const auto& evt : ctx.input_event_queue) {
        std::visit(
            [](auto&& event) {
                using T = std::decay_t<decltype(event)>;
                if constexpr (std::is_same_v<T, evts::KeyPressed>) {
                    LOG_DEBUG("Key Pressed: {}", static_cast<int>(event.keycode));
                } else if constexpr (std::is_same_v<T, evts::KeyHold>) {
                    LOG_DEBUG("Key Held: {}", static_cast<int>(event.keycode));
                } else if constexpr (std::is_same_v<T, evts::KeyReleased>) {
                    LOG_DEBUG("Key Released: {}", static_cast<int>(event.keycode));
                } else if constexpr (std::is_same_v<T, evts::MouseButtonPressed>) {
                    LOG_DEBUG("Mouse Button Pressed: {}", static_cast<int>(event.button));
                } else if constexpr (std::is_same_v<T, evts::MouseButtonHeld>) {
                    LOG_DEBUG("Mouse Button Held: {}", static_cast<int>(event.button));
                } else if constexpr (std::is_same_v<T, evts::MouseButtonReleased>) {
                    LOG_DEBUG("Mouse Button Released: {}", static_cast<int>(event.button));
                } else if constexpr (std::is_same_v<T, evts::MouseMoved>) {
                    LOG_DEBUG("Mouse Moved to: ({}, {})", event.x, event.y);
                } else if constexpr (std::is_same_v<T, evts::MouseScrolled>) {
                    LOG_DEBUG("Mouse Scrolled: offset({}, {})", event.offset_x, event.offset_y);
                } else if constexpr (std::is_same_v<T, evts::ControllerButtonPressed>) {
                    LOG_DEBUG("Controller Button Pressed: {}", static_cast<int>(event.button));
                } else if constexpr (std::is_same_v<T, evts::ControllerButtonHeld>) {
                    LOG_DEBUG("Controller Button Held: {}", static_cast<int>(event.button));
                } else if constexpr (std::is_same_v<T, evts::ControllerButtonReleased>) {
                    LOG_DEBUG("Controller Button Released: {}", static_cast<int>(event.button));
                } else if constexpr (std::is_same_v<T, evts::ControllerLeftJoystickMove>) {
                    LOG_DEBUG("Controller Left Joystick Moved to: ({}, {})", event.x, event.y);
                } else if constexpr (std::is_same_v<T, evts::ControllerRightJoystickMove>) {
                    LOG_DEBUG("Controller Right Joystick Moved to: ({}, {})", event.x, event.y);
                } else if constexpr (std::is_same_v<T, evts::ControllerLeftTriggerMove>) {
                    LOG_DEBUG("Controller Left Trigger Moved to: {}", event.value);
                } else if constexpr (std::is_same_v<T, evts::ControllerRightTriggerMove>) {
                    LOG_DEBUG("Controller Right Trigger Moved to: {}", event.value);
                } else {
                    LOG_WARNING("Unknown input event received.");
                }
            },
            evt);
    }
}
