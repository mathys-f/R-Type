#include "systems/systems.h"
#include "engine.h"

using namespace engn;

void sys::ui_press(EngineContext &ctx)
{
    auto &interacts = ctx.registry.get_components<cpnt::UIInteractable>();

    // Retrieve currently focused entity
    auto entity = ctx.registry.tag_registry.get_entity(ctx.focused_entity);
    if (!entity.has_value()) return;

    // Check bounds before accessing the vector
    auto entity_idx = entity.value();
    if (entity_idx >= interacts.size()) return;

    // Retreive the UIInteractable component of the currently focused entity
    auto &interact = interacts[entity_idx];
    if (!interact.has_value()) return;

    ctx.input_event_queue.for_each<evts::MouseButtonPressed>(
        [&](const evts::MouseButtonPressed &evt) {
            if (evt.button != evts::mouse_button_left) return;
            if (!interact.value().hovered) return;

            interact.value().pressed = true;
            interact.value().released = false;
        }
    );

    ctx.input_event_queue.for_each<evts::MouseButtonReleased>(
        [&](const evts::MouseButtonReleased &evt) {
            if (evt.button != evts::mouse_button_left) return;

            // Check bounds again before accessing in lambda
            if (entity_idx >= interacts.size()) return;
            auto &interact = interacts[entity_idx];

            if (!interact.has_value()) return;
            if (interact.value().hovered)
                interact.value().released = true;
            interact.value().pressed = false;
        }
    );
}
