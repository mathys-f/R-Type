#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"
#include "utils/logger.h"

#include <raylib.h>

#include <algorithm>
#include <cmath>

using namespace engn;

namespace {
constexpr float k_text_spacing = 1.0f;
constexpr float k_blink_period = 1.0f;
constexpr float k_half = 2.0f;
} // namespace

void sys::ui_text_renderer(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms,
                           const ecs::SparseArray<cpnt::UIText>& texts, const ecs::SparseArray<cpnt::UIStyle>& styles,
                           const ecs::SparseArray<cpnt::UIInteractable>& interactables) {
    const ecs::Registry& reg = ctx.registry;
    const auto& input_fields = ctx.registry.get_components<cpnt::UIInputField>();
    const float k_width = ctx.window_size.x;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

    for (const auto& [index, transform, text, style, interactable] :
         ecs::indexed_zipper(transforms, texts, styles, interactables)) {
        if (index == 0)
            continue;

        if (!transform.has_value() || !text.has_value() || !style.has_value())
            continue;

        Vector2 string_size =
            MeasureTextEx(GetFontDefault(), text->content.c_str(), static_cast<float>(text->font_size), k_text_spacing);
        Vector2 position;
        position.x = (transform->x / 100.0f * k_width)
            + ((transform->w / 100.0f * k_width) - string_size.x) / 2.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
        position.y = (transform->y / 100.0f * k_height)
            + ((transform->h / 100.0f * k_height) - string_size.y) / 2.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers)

        Color text_color{style->text_color.r, style->text_color.g, style->text_color.b, style->text_color.a};

        if (interactable.has_value()) {
            if (interactable->pressed) {
                text_color = {style->text_color_pressed.r, style->text_color_pressed.g, style->text_color_pressed.b,
                              style->text_color_pressed.a};
            } else if (interactable->hovered) {
                text_color = {style->text_color_hovered.r, style->text_color_hovered.g, style->text_color_hovered.b,
                              style->text_color_hovered.a};
            }
        }

        std::string display_text = text->content;
        if (index < input_fields.size()) {
            const auto& input_field = input_fields[index];
            if (input_field.has_value() && input_field->editing) {
                if (std::fmod(input_field->timer, k_blink_period) < (k_blink_period / k_half)) {
                    size_t cursor = std::min(input_field->cursor_index, display_text.size());
                    display_text.insert(cursor, "|");
                }
            }
        }

        DrawText(display_text.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), text->font_size,
                 text_color);
    }
}
