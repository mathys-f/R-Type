#include "engine.h"
#include "events/events.h"
#include "systems/systems.h"

#include <algorithm>
#include <cmath>

using namespace engn;

namespace {

constexpr float k_axis_deadzone = 0.1f;

bool binding_matches_key(const ControlBinding& binding, evts::KeyboardKeyCode keycode) {
    return binding.primary == keycode || binding.secondary == keycode;
}

bool binding_matches_button(const GamepadControlBinding& binding, evts::ControllerButton button) {
    return binding.primary == button || binding.secondary == button;
}

bool is_binding_held(const evts::EventQueue<evts::Event>& events, const ControlBinding& binding) {
    bool down = false;
    events.for_each<evts::KeyHold>([&](const evts::KeyHold& evt) {
        if (binding_matches_key(binding, evt.keycode)) {
            down = true;
        }
    });
    return down;
}

bool is_binding_pressed(const evts::EventQueue<evts::Event>& events, const ControlBinding& binding) {
    bool pressed = false;
    events.for_each<evts::KeyPressed>([&](const evts::KeyPressed& evt) {
        if (binding_matches_key(binding, evt.keycode)) {
            pressed = true;
        }
    });
    return pressed;
}

bool is_gamepad_binding_held(const evts::EventQueue<evts::Event>& events, const GamepadControlBinding& binding) {
    bool down = false;
    events.for_each<evts::ControllerButtonHeld>([&](const evts::ControllerButtonHeld& evt) {
        if (binding_matches_button(binding, evt.button)) {
            down = true;
        }
    });
    return down;
}

bool is_gamepad_binding_pressed(const evts::EventQueue<evts::Event>& events, const GamepadControlBinding& binding) {
    bool pressed = false;
    events.for_each<evts::ControllerButtonPressed>([&](const evts::ControllerButtonPressed& evt) {
        if (binding_matches_button(binding, evt.button)) {
            pressed = true;
        }
    });
    return pressed;
}

} // namespace

void sys::resolve_player_input(EngineContext& ctx) {
    InputState state{};
    const auto& events = ctx.input_event_queue;

    if (is_binding_held(events, ctx.controls.move_right))
        state.move_x += 1.0f;
    if (is_binding_held(events, ctx.controls.move_left))
        state.move_x -= 1.0f;
    if (is_binding_held(events, ctx.controls.move_down))
        state.move_y += 1.0f;
    if (is_binding_held(events, ctx.controls.move_up))
        state.move_y -= 1.0f;

    if (is_binding_pressed(events, ctx.controls.shoot))
        state.shoot_pressed = true;
    if (is_binding_held(events, ctx.controls.shoot))
        state.shoot_held = true;

    const auto* left_stick = events.get_last<evts::ControllerLeftJoystickMove>();
    if (left_stick) {
        if (std::abs(left_stick->x) >= k_axis_deadzone)
            state.move_x += left_stick->x;
        if (std::abs(left_stick->y) >= k_axis_deadzone)
            state.move_y += left_stick->y;
        state.using_gamepad = true;
    }

    if (is_gamepad_binding_held(events, ctx.gamepad_controls.move_right)) {
        state.move_x += 1.0f;
        state.using_gamepad = true;
    }
    if (is_gamepad_binding_held(events, ctx.gamepad_controls.move_left)) {
        state.move_x -= 1.0f;
        state.using_gamepad = true;
    }
    if (is_gamepad_binding_held(events, ctx.gamepad_controls.move_down)) {
        state.move_y += 1.0f;
        state.using_gamepad = true;
    }
    if (is_gamepad_binding_held(events, ctx.gamepad_controls.move_up)) {
        state.move_y -= 1.0f;
        state.using_gamepad = true;
    }
    if (is_gamepad_binding_pressed(events, ctx.gamepad_controls.shoot)) {
        state.shoot_pressed = true;
        state.using_gamepad = true;
    }
    if (is_gamepad_binding_held(events, ctx.gamepad_controls.shoot)) {
        state.shoot_held = true;
        state.using_gamepad = true;
    }

    state.move_x = std::clamp(state.move_x, -1.0f, 1.0f);
    state.move_y = std::clamp(state.move_y, -1.0f, 1.0f);

    ctx.input_state = state;
}
