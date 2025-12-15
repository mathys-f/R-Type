#include "systems/systems.h"

#include <raylib.h>

#include "engine.h"
#include "ecs/zipper.h"

using namespace engn;

void sys::ui_text_renderer(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &transforms,
    const ecs::SparseArray<cpnt::UIText> &texts,
    const ecs::SparseArray<cpnt::UIStyle> &styles
)
{
    const ecs::Registry &reg = ctx.registry;

    for (const auto &[transform, text, style] :
        ecs::zipper(transforms, texts, styles)) {
        Color text_color{
            style->text_color.r,
            style->text_color.g,
            style->text_color.b,
            style->text_color.a
        };

        Vector2 position{
            transform->x,
            transform->y
        };

        DrawText(text->content.c_str(),
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            text->font_size, text_color);
    }
}
