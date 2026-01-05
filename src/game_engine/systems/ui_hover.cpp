#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"
#include "utils/logger.h"

#include <algorithm>

using namespace engn;

void sys::ui_hover(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms) {
    auto& interacts = ctx.registry.get_components<cpnt::UIInteractable>();
    const evts::MouseMoved* mouse_pos = ctx.input_event_queue.get_last<evts::MouseMoved>();
    bool has_found = false;

    if (!mouse_pos)
        return;

    for (size_t i = 0; i < interacts.size(); i++) {
        if (!transforms[i].has_value() || !interacts[i].has_value())
            continue;

        const auto& t = transforms[i].value();

        if (mouse_pos->x >= t.x && mouse_pos->x <= t.x + t.w && mouse_pos->y >= t.y && mouse_pos->y <= t.y + t.h) {
            interacts[i].value().hovered = true;

            ctx.focused_entity = ctx.registry.entity_from_index(i);
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            has_found = true;
        } else {
            interacts[i].value().hovered = false;
        }
    }

    if (!has_found)
        SetMouseCursor(MOUSE_CURSOR_ARROW);
}
