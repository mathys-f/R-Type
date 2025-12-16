#include "systems/systems.h"
#include "engine.h"

using namespace engn;

void sys::ui_press(EngineContext &ctx)
{
    auto &interacts = ctx.registry.get_components<cpnt::UIInteractable>();
    auto &ui_evts_queue = ctx.ui_event_queue;

    // Retrieve currently focused entity
    auto entity = ctx.registry.tag_registry.get_entity(ctx.focused_entity);
    if (!entity.has_value()) return;

    // Check bounds before accessing the vector
    auto entity_idx = entity.value();
    if (entity_idx >= interacts.size()) return;

    // Retreive the UIInteractable component of the currently focused entity
    auto &interact = interacts[entity_idx];
    if (!interact.has_value()) return;

    // Check for mouse button press events
    auto mouse_pressed = ctx.input_event_queue.get_first<evts::MouseButtonPressed>();
    if (mouse_pressed && mouse_pressed->button == evts::mouse_button_left) {
        if (!interact.value().hovered) return;

        interact->pressed = true;
    }

    // Check for mouse button release events
    auto mouse_released = ctx.input_event_queue.get_first<evts::MouseButtonReleased>();
    if (mouse_released && mouse_released->button == evts::mouse_button_left) {
        if (!interact.value().pressed) return;
        if (!interact.value().hovered) return;

        ui_evts_queue.push(evts::UIButtonClicked{ctx.registry.tag_registry.get_tag_id(ctx.focused_entity)});
        interact->pressed = false;
    }
}
