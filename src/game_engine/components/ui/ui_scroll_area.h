#pragma once

#include "ecs/entity.h"
#include "utils/color.h"

namespace engn {

namespace cpnt {

struct UIScrollArea {
    float content_height_px = 0.0f;
    float scroll_offset_px = 0.0f;
    float scroll_step_px = 50.0f;
    float padding_px = 0.0f;

    float scrollbar_width_px = 8.0f;
    float scrollbar_padding_px = 2.0f;
    float scrollbar_min_thumb_px = 14.0f;
    float scrollbar_radius = 0.2f;
    float scrollbar_border_thickness = 0.0f;
    unsigned char scrollbar_hidden_alpha = 0U;
    utils::Color scrollbar_track_color{25, 25, 25, 140};
    utils::Color scrollbar_thumb_color{140, 140, 140, 220};

    ecs::Entity scrollbar_track_entity{};
    ecs::Entity scrollbar_thumb_entity{};

    bool dragging = false;
    float drag_offset_px = 0.0f;
    float max_scroll_px = 0.0f;
    bool visible = false;

    float track_x_px = 0.0f;
    float track_y_px = 0.0f;
    float track_w_px = 0.0f;
    float track_h_px = 0.0f;
    float thumb_y_px = 0.0f;
    float thumb_h_px = 0.0f;
};

} // namespace cpnt

} // namespace engn
