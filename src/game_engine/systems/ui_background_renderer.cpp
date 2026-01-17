#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"
#include "components/ui/ui_clip.h"

#include <raylib.h>

using namespace engn;

void sys::ui_background_renderer(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms,
                                 const ecs::SparseArray<cpnt::UIStyle>& styles,
                                 const ecs::SparseArray<cpnt::UIInteractable>& interactables) {
    const ecs::Registry& reg = ctx.registry;
    auto& clips = ctx.registry.register_component<cpnt::UIClip>();
    const float k_width = ctx.window_size.x;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

    for (const auto& [index, transform, style, interactable] : ecs::indexed_zipper(transforms, styles, interactables)) {
        if (index == 0)
            continue;

        if (!transform.has_value() || !style.has_value())
            continue;

        Rectangle rect{
            transform->x / 100.0f * k_width,
            transform->y / 100.0f * k_height,
            transform->w / 100.0f * k_width,
            transform->h / 100.0f * k_height
        };
        Color rect_color{style->background_color.r, style->background_color.g, style->background_color.b,
                         style->background_color.a};
        Color border_color{style->border_color.r, style->border_color.g, style->border_color.b, style->border_color.a};

        if (interactable.has_value()) {
            if (interactable->pressed) {
                rect_color = {style->background_color_pressed.r, style->background_color_pressed.g,
                              style->background_color_pressed.b, style->background_color_pressed.a};
                border_color = {style->border_color_pressed.r, style->border_color_pressed.g,
                                style->border_color_pressed.b, style->border_color_pressed.a};
            } else if (interactable->hovered) {
                rect_color = {style->background_color_hovered.r, style->background_color_hovered.g,
                              style->background_color_hovered.b, style->background_color_hovered.a};
                border_color = {style->border_color_hovered.r, style->border_color_hovered.g,
                                style->border_color_hovered.b, style->border_color_hovered.a};
            }
        }

        const bool k_has_clip = index < clips.size() && clips[index].has_value();
        if (k_has_clip) {
            const auto& clip = clips[index].value();
            const int k_clip_x = static_cast<int>(clip.x / 100.0f * k_width);
            const int k_clip_y = static_cast<int>(clip.y / 100.0f * k_height);
            const int k_clip_w = static_cast<int>(clip.w / 100.0f * k_width);
            const int k_clip_h = static_cast<int>(clip.h / 100.0f * k_height);
            BeginScissorMode(k_clip_x, k_clip_y, k_clip_w, k_clip_h);
        }

        DrawRectangleRounded(rect, style->border_radius, 0.0f, rect_color);
        DrawRectangleRoundedLinesEx(rect, style->border_radius, 0.0f, style->border_thickness, border_color);

        if (k_has_clip) {
            EndScissorMode();
        }
    }
}
