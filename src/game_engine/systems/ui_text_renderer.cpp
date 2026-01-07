#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"
#include "utils/logger.h"

#include <raylib.h>

using namespace engn;

namespace {
constexpr float k_text_spacing = 1.0f;
constexpr float k_center_divisor = 2.0f;
} // namespace

void sys::ui_text_renderer(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms,
                           const ecs::SparseArray<cpnt::UIText>& texts, const ecs::SparseArray<cpnt::UIStyle>& styles) {
    const ecs::Registry& reg = ctx.registry;

    for (const auto& [index, transform, text, style] : ecs::indexed_zipper(transforms, texts, styles)) {
        if (index == 0)
            continue;

        if (!transform.has_value() || !text.has_value() || !style.has_value())
            continue;

        Vector2 string_size =
            MeasureTextEx(GetFontDefault(), text->content.c_str(), static_cast<float>(text->font_size), k_text_spacing);
        Vector2 position;
        position.x = transform->x + (transform->w - string_size.x) / k_center_divisor;
        position.y = transform->y + (transform->h - string_size.y) / k_center_divisor;

        Color text_color{style->text_color.r, style->text_color.g, style->text_color.b, style->text_color.a};

        DrawText(text->content.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), text->font_size,
                 text_color);
    }
}
