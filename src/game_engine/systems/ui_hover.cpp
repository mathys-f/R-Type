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
    const float k_width = ctx.k_window_size.x; // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.k_window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

    if (!mouse_pos)
        return;

    for (size_t i = 0; i < interacts.size(); i++) {
        if (!transforms[i].has_value() || !interacts[i].has_value())
            continue;

        const auto& t = transforms[i].value();
        const float k_x = (t.x / 100.0f) * k_width;
        const float k_y = (t.y / 100.0f) * k_height;
        const float k_w = (t.w / 100.0f) * k_width;
        const float k_h = (t.h / 100.0f) * k_height;

        if (mouse_pos->x >= k_x && mouse_pos->x <= k_x + k_w && mouse_pos->y >= k_y && mouse_pos->y <= k_y + k_h) {
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
