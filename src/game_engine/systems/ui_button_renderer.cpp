#include "systems/systems.h"

#include <raylib.h>

#include "engine.h"
#include "ecs/zipper.h"
#include "utils/logger.h"

using namespace engn;

void sys::ui_background_renderer(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &transforms,
    const ecs::SparseArray<cpnt::UIStyle> &styles
)
{
    const ecs::Registry &reg = ctx.registry;

    for (const auto &[transform, style] :
        ecs::zipper(transforms, styles)) {
        Rectangle rect{
            transform->x,
            transform->y,
            transform->w,
            transform->h
        };
        Color rect_color{
            style->background_color.r,
            style->background_color.g,
            style->background_color.b,
            style->background_color.a
        };
        Color border_color{
            style->border_color.r,
            style->border_color.g,
            style->border_color.b,
            style->border_color.a
        };

        DrawRectangleRounded(rect, style->border_radius, 0.0f, rect_color);
        DrawRectangleRoundedLinesEx(rect, style->border_radius, 0.0f, style->border_thickness, border_color);
    }
}
