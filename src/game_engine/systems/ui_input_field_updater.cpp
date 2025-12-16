#include "systems/systems.h"

#include "engine.h"
#include "ecs/zipper.h"
#include <cmath>

using namespace engn;

static bool left_mouse_button_pressed(engn::EngineContext& ctx)
{
    const evts::MouseButtonPressed* mouse_evt = ctx.input_event_queue.get_last<evts::MouseButtonPressed>();
    return mouse_evt && mouse_evt->button == evts::mouse_button_left;
}

// Helper function to convert keycode to character
static char keycode_to_char(evts::KeyboardKeyCode keycode) {
    if (keycode >= evts::key_a && keycode <= evts::key_z) {
        char base_char = 'a' + (keycode - evts::key_a);
        return base_char;
    }
    if (keycode >= evts::key_0 && keycode <= evts::key_9) {
        return '0' + (keycode - evts::key_0);
    }
    if (keycode == evts::key_period) return '.';
    if (keycode == evts::key_space) return ' ';

    return '\0'; // Invalid character
}

void sys::ui_input_field_updater(engn::EngineContext& ctx,
    const ecs::SparseArray<cpnt::UIInteractable>& interactables)
{
    auto& reg = ctx.registry;
    auto& evts = ctx.input_event_queue;
    float delta_time = ctx.delta_time;
    auto& texts = reg.get_components<cpnt::UIText>();
    auto& inputs = reg.get_components<cpnt::UIInputField>();

    for (size_t i = 0; i < inputs.size(); i++) {
        auto &input = inputs[i];
        auto &text = texts[i];
        const auto &inter = interactables[i];

        if (!input.has_value() || !text.has_value() || !inter.has_value())
            continue;

        // Start editing when clicked
        if (inter->pressed && !input->editing) {
            input->editing = true;
            input->timer = 0.0f;
        }

        // Stop editing when clicked outside or Enter is pressed
        if (input->editing) {
            const evts::KeyPressed* key_evt = evts.get_last<evts::KeyPressed>();
            if (key_evt && key_evt->keycode == evts::key_enter) {
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

        if (!input->editing)
            continue;
        // Update cursor blink timer
        input->timer += delta_time;

        // Handle text input
        const evts::KeyPressed* key_pressed = evts.get_last<evts::KeyPressed>();
        if (key_pressed) {
            LOG_DEBUG("Key pressed: {}", static_cast<int>(key_pressed->keycode));
            if (key_pressed->keycode == evts::key_backspace) {
                // Remove last character
                if (!text->content.empty()) {
                    text->content.pop_back();
                }
            } else {
                // Add character if valid
                char new_char = keycode_to_char(key_pressed->keycode);
                if (new_char != '\0') {
                    text->content += new_char;
                }
            }
        }
    }
}
