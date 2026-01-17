#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

#include <raylib.h>

using namespace engn;

void sys::ui_background_renderer(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms,
                                 const ecs::SparseArray<cpnt::UIStyle>& styles,
                                 const ecs::SparseArray<cpnt::UIInteractable>& interactables) {
    const ecs::Registry& reg = ctx.registry;
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

        DrawRectangleRounded(rect, style->border_radius, 0.0f, rect_color);
        DrawRectangleRoundedLinesEx(rect, style->border_radius, 0.0f, style->border_thickness, border_color);
    }
}
