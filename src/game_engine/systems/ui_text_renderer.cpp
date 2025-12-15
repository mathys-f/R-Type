#include "systems/systems.h"

#include <raylib.h>

#include "engine.h"
#include "ecs/zipper.h"
#include "utils/logger.h"

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
        Vector2 string_size = MeasureTextEx(GetFontDefault(), text->content.c_str(), text->font_size, 1.0f);
        Vector2 position;
        position.x = transform->x + (transform->w - string_size.x) / 2.0f;
        position.y = transform->y + (transform->h - string_size.y) / 2.0f;

        Color text_color{
            style->text_color.r,
            style->text_color.g,
            style->text_color.b,
            style->text_color.a
        };

        DrawText(text->content.c_str(),
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            text->font_size, text_color);
    }
}
