#include "engine.h"
#include "ecs/zipper.h"
#include "systems/systems.h"

#include <raylib.h>

#include <algorithm>

using namespace engn;

namespace {

ecs::Entity get_or_create_scrollbar_entity(EngineContext& ctx, ecs::Entity& cached_entity) {
    if (cached_entity.value() != 0) {
        return cached_entity;
    }
    cached_entity = ctx.registry.spawn_entity();
    return cached_entity;
}

bool point_in_rect(float x, float y, float rx, float ry, float rw, float rh) {
    return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh);
}

} // namespace

void sys::ui_scroll_area(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>& transforms,
                         const ecs::SparseArray<cpnt::UIScrollArea>& scroll_areas) {
    auto& scroll_areas_mut = ctx.registry.get_components<cpnt::UIScrollArea>();
    auto& interactables = ctx.registry.get_components<cpnt::UIInteractable>();

    const float k_width = ctx.window_size.x;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

    const auto* k_scroll_evt = ctx.input_event_queue.get_last<evts::MouseScrolled>();
    const auto* k_mouse_pressed = ctx.input_event_queue.get_last<evts::MouseButtonPressed>();
    const auto* k_mouse_released = ctx.input_event_queue.get_last<evts::MouseButtonReleased>();
    const float k_mouse_x = static_cast<float>(GetMouseX());
    const float k_mouse_y = static_cast<float>(GetMouseY());

    for (const auto& [index, transform_opt, scroll_area_opt] :
         ecs::indexed_zipper(transforms, scroll_areas)) {
        if (index == 0)
            continue;
        if (!transform_opt.has_value() || !scroll_area_opt.has_value())
            continue;

        auto& area = scroll_areas_mut[index].value();
        const auto& transform = transform_opt.value();

        const float k_rect_x = (transform.x / 100.0f) * k_width;
        const float k_rect_y = (transform.y / 100.0f) * k_height;
        const float k_rect_w = (transform.w / 100.0f) * k_width;
        const float k_rect_h = (transform.h / 100.0f) * k_height;

        const float k_viewport_h_px = std::max(0.0f, k_rect_h - (area.padding_px * 2.0f));
        const float k_content_h_px = std::max(0.0f, area.content_height_px);
        const float k_max_scroll_px = std::max(0.0f, k_content_h_px - k_viewport_h_px);

        area.max_scroll_px = k_max_scroll_px;
        area.visible = k_max_scroll_px > 0.0f;
        if (!area.visible) {
            area.scroll_offset_px = 0.0f;
            area.dragging = false;
            area.drag_offset_px = 0.0f;
        } else {
            area.scroll_offset_px = std::clamp(area.scroll_offset_px, 0.0f, k_max_scroll_px);
        }

        const float k_track_x = k_rect_x + k_rect_w - area.scrollbar_width_px - area.scrollbar_padding_px;
        const float k_track_y = k_rect_y + area.padding_px;
        const float k_track_w = area.scrollbar_width_px;
        const float k_track_h = std::max(0.0f, k_viewport_h_px);

        float k_thumb_h = k_track_h;
        if (area.visible && k_content_h_px > 0.0f) {
            k_thumb_h = k_track_h * (k_viewport_h_px / k_content_h_px);
            k_thumb_h = std::max(area.scrollbar_min_thumb_px, k_thumb_h);
            k_thumb_h = std::min(k_thumb_h, k_track_h);
        }

        float k_thumb_y = k_track_y;
        if (area.visible && k_max_scroll_px > 0.0f && k_track_h > k_thumb_h) {
            const float k_scroll_ratio = area.scroll_offset_px / k_max_scroll_px;
            k_thumb_y = k_track_y + (k_track_h - k_thumb_h) * k_scroll_ratio;
        }

        area.track_x_px = k_track_x;
        area.track_y_px = k_track_y;
        area.track_w_px = k_track_w;
        area.track_h_px = k_track_h;
        area.thumb_y_px = k_thumb_y;
        area.thumb_h_px = k_thumb_h;

        if (area.visible) {
            if (k_mouse_pressed && k_mouse_pressed->button == evts::MouseButton::MouseButtonLeft) {
                if (point_in_rect(k_mouse_x, k_mouse_y, k_track_x, k_thumb_y, k_track_w, k_thumb_h)) {
                    area.dragging = true;
                    area.drag_offset_px = k_mouse_y - k_thumb_y;
                }
            }
            if (k_mouse_released && k_mouse_released->button == evts::MouseButton::MouseButtonLeft) {
                area.dragging = false;
            }

            if (area.dragging && k_track_h > k_thumb_h) {
                const float k_relative_y = (k_mouse_y - area.drag_offset_px) - k_track_y;
                const float k_max_thumb_travel = std::max(0.0f, k_track_h - k_thumb_h);
                const float k_ratio = std::clamp(k_relative_y / k_max_thumb_travel, 0.0f, 1.0f);
                area.scroll_offset_px = k_ratio * k_max_scroll_px;
            } else if (k_scroll_evt &&
                       point_in_rect(k_mouse_x, k_mouse_y, k_rect_x, k_rect_y, k_rect_w, k_rect_h)) {
                area.scroll_offset_px = std::clamp(
                    area.scroll_offset_px - (k_scroll_evt->offset_y * area.scroll_step_px), 0.0f, k_max_scroll_px);
            }
        }

        utils::Color track_color = area.scrollbar_track_color;
        utils::Color thumb_color = area.scrollbar_thumb_color;
        if (!area.visible) {
            track_color.a = area.scrollbar_hidden_alpha;
            thumb_color.a = area.scrollbar_hidden_alpha;
        }

        const ecs::Entity k_track_entity = get_or_create_scrollbar_entity(ctx, area.scrollbar_track_entity);
        ctx.registry.add_component(
            k_track_entity,
            cpnt::UITransform{(k_track_x / k_width) * 100.0f, (k_track_y / k_height) * 100.0f, transform.z,
                              (k_track_w / k_width) * 100.0f, (k_track_h / k_height) * 100.0f, 0.0f, 0.0f, 0.0f});
        ctx.registry.add_component(k_track_entity,
                                   cpnt::UIStyle{track_color, track_color, track_color,
                                                 utils::Color{0U, 0U, 0U, 0U}, utils::Color{0U, 0U, 0U, 0U},
                                                 utils::Color{0U, 0U, 0U, 0U}, utils::Color{0U, 0U, 0U, 0U},
                                                 utils::Color{0U, 0U, 0U, 0U}, utils::Color{0U, 0U, 0U, 0U},
                                                 area.scrollbar_radius, area.scrollbar_border_thickness});
        ctx.registry.add_component(k_track_entity, cpnt::UIInteractable{});

        const ecs::Entity k_thumb_entity = get_or_create_scrollbar_entity(ctx, area.scrollbar_thumb_entity);
        ctx.registry.add_component(
            k_thumb_entity,
            cpnt::UITransform{(k_track_x / k_width) * 100.0f, (k_thumb_y / k_height) * 100.0f, transform.z,
                              (k_track_w / k_width) * 100.0f, (k_thumb_h / k_height) * 100.0f, 0.0f, 0.0f, 0.0f});
        ctx.registry.add_component(k_thumb_entity,
                                   cpnt::UIStyle{thumb_color, thumb_color, thumb_color,
                                                 utils::Color{0U, 0U, 0U, 0U}, utils::Color{0U, 0U, 0U, 0U},
                                                 utils::Color{0U, 0U, 0U, 0U}, utils::Color{0U, 0U, 0U, 0U},
                                                 utils::Color{0U, 0U, 0U, 0U}, utils::Color{0U, 0U, 0U, 0U},
                                                 area.scrollbar_radius, area.scrollbar_border_thickness});
        ctx.registry.add_component(k_thumb_entity, cpnt::UIInteractable{});
    }
}
