#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

#include <cmath>

using namespace engn;

static bool left_mouse_button_pressed(engn::EngineContext& ctx) {
    const evts::MouseButtonPressed* mouse_evt = ctx.input_event_queue.get_last<evts::MouseButtonPressed>();
    return mouse_evt && mouse_evt->button == evts::MouseButton::MouseButtonLeft;
}

// Helper function to convert keycode to character
static char keycode_to_char(evts::KeyboardKeyCode keycode) {
    if (keycode >= evts::KeyboardKeyCode::KeyA && keycode <= evts::KeyboardKeyCode::KeyZ) {
        char base_char =
            static_cast<char>('a' + (static_cast<int>(keycode) - static_cast<int>(evts::KeyboardKeyCode::KeyA)));
        return base_char;
    }
    if (keycode >= evts::KeyboardKeyCode::Key0 && keycode <= evts::KeyboardKeyCode::Key9) {
        return static_cast<char>('0' + (static_cast<int>(keycode) - static_cast<int>(evts::KeyboardKeyCode::Key0)));
    }
    if (keycode == evts::KeyboardKeyCode::KeyPeriod)
        return '.';
    if (keycode == evts::KeyboardKeyCode::KeySpace)
        return ' ';

    return '\0'; // Invalid character
}

void sys::ui_input_field_updater(engn::EngineContext& ctx,
                                 const ecs::SparseArray<cpnt::UIInteractable>& interactables) {
    auto& reg = ctx.registry;
    auto& evts = ctx.input_event_queue;
    float delta_time = ctx.delta_time;
    auto& texts = reg.get_components<cpnt::UIText>();
    auto& inputs = reg.get_components<cpnt::UIInputField>();

    for (size_t i = 0; i < inputs.size(); i++) {
        auto& input = inputs[i];
        auto& text = texts[i];
        const auto& inter = interactables[i];

        if (!input.has_value() || !text.has_value() || !inter.has_value())
            continue;

        // Focus input when clicked (editing starts with Enter)
        if (inter->pressed) {
            ctx.focused_entity = reg.entity_from_index(i);
        }

        // Stop editing when clicked outside or Enter is pressed
        if (input->editing) {
            const evts::KeyPressed* key_evt = evts.get_last<evts::KeyPressed>();
            if (key_evt && key_evt->keycode == evts::KeyboardKeyCode::KeyEnter) {
                input->editing = false;
                LOG_DEBUG("Submitted input field with text: {}", text->content);
                continue;
            }

            // Stop editing if clicked elsewhere (this would need UI focus management)
            if (left_mouse_button_pressed(ctx) && !inter->hovered) {
                input->editing = false;
                LOG_DEBUG("Stopped editing input field");
                continue;
            }
        }

        if (!input->editing) {
            const evts::KeyPressed* key_evt = evts.get_last<evts::KeyPressed>();
            if (key_evt && key_evt->keycode == evts::KeyboardKeyCode::KeyEnter &&
                ctx.focused_entity == reg.entity_from_index(i)) {
                input->editing = true;
                input->timer = 0.0f;
                input->cursor_index = text->content.size();
            }
        }

        if (!input->editing)
            continue;
        // Update cursor blink timer
        input->timer += delta_time;

        // Handle text input
        const evts::KeyPressed* key_pressed = evts.get_last<evts::KeyPressed>();
        if (key_pressed) {
            LOG_DEBUG("Key pressed: {}", static_cast<int>(key_pressed->keycode));
            if (key_pressed->keycode == evts::KeyboardKeyCode::KeyBackspace) {
                // Remove last character
                if (input->cursor_index > 0 && !text->content.empty()) {
                    text->content.erase(input->cursor_index - 1, 1);
                    input->cursor_index--;
                }
            } else if (key_pressed->keycode == evts::KeyboardKeyCode::KeyLeft) {
                if (input->cursor_index > 0)
                    input->cursor_index--;
            } else if (key_pressed->keycode == evts::KeyboardKeyCode::KeyRight) {
                if (input->cursor_index < text->content.size())
                    input->cursor_index++;
            } else {
                // Add character if valid
                char new_char = keycode_to_char(key_pressed->keycode);
                if (new_char != '\0') {
                    text->content.insert(text->content.begin() + static_cast<long>(input->cursor_index), new_char);
                    input->cursor_index++;
                }
            }
        }
    }
}
