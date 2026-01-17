#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"
#include "utils/logger.h"
#include "components/ui/ui_clip.h"

#include <algorithm>

using namespace engn;

void sys::ui_hover(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms) {
    auto& interacts = ctx.registry.get_components<cpnt::UIInteractable>();
    auto& clips = ctx.registry.register_component<cpnt::UIClip>();
    const evts::MouseMoved* mouse_pos = ctx.input_event_queue.get_last<evts::MouseMoved>();
    bool has_found = false;
    const float k_width = ctx.window_size.x; // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

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

        const bool k_has_clip = i < clips.size() && clips[i].has_value();
        if (k_has_clip) {
            const auto& clip = clips[i].value();
            const float k_clip_x = (clip.x / 100.0f) * k_width;
            const float k_clip_y = (clip.y / 100.0f) * k_height;
            const float k_clip_w = (clip.w / 100.0f) * k_width;
            const float k_clip_h = (clip.h / 100.0f) * k_height;
            const bool k_in_clip = (mouse_pos->x >= k_clip_x && mouse_pos->x <= k_clip_x + k_clip_w &&
                                    mouse_pos->y >= k_clip_y && mouse_pos->y <= k_clip_y + k_clip_h);
            if (!k_in_clip) {
                interacts[i].value().hovered = false;
                continue;
            }
        }

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
