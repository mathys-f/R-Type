local function rgba(r, g, b, a)
    return { r = r, g = g, b = b, a = a }
end

local function style(bg, bg_hovered, bg_pressed, txt, txt_hovered, txt_pressed, border, border_hovered, border_pressed, border_radius, border_thickness)
    return {
        background_color = bg,
        background_color_hovered = bg_hovered,
        background_color_pressed = bg_pressed,
        text_color = txt,
        text_color_hovered = txt_hovered,
        text_color_pressed = txt_pressed,
        border_color = border,
        border_color_hovered = border_hovered,
        border_color_pressed = border_pressed,
        border_radius = border_radius,
        border_thickness = border_thickness
    }
end

UITheme = {
    presets = {
        main_menu = {
            main_menu = style(
                rgba(20, 30, 36, 235), rgba(30, 46, 54, 245), rgba(14, 20, 24, 255),
                rgba(205, 238, 246, 255), rgba(236, 250, 255, 255), rgba(162, 210, 226, 255),
                rgba(72, 132, 150, 255), rgba(118, 196, 220, 255), rgba(152, 226, 245, 255),
                0.45, 4
            ),
            titlescreen = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(194, 235, 250, 255), rgba(223, 247, 255, 255), rgba(158, 208, 224, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            )
        },
        lobby = {
            main_menu = style(
                rgba(20, 30, 36, 235), rgba(30, 46, 54, 245), rgba(14, 20, 24, 255),
                rgba(205, 238, 246, 255), rgba(236, 250, 255, 255), rgba(162, 210, 226, 255),
                rgba(72, 132, 150, 255), rgba(118, 196, 220, 255), rgba(152, 226, 245, 255),
                0.45, 4
            ),
            title = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(186, 228, 242, 255), rgba(210, 241, 252, 255), rgba(148, 196, 214, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            lobby_item = style(
                rgba(18, 25, 29, 255), rgba(25, 37, 43, 255), rgba(12, 18, 21, 255),
                rgba(206, 230, 238, 255), rgba(234, 248, 255, 255), rgba(168, 198, 210, 255),
                rgba(65, 94, 106, 255), rgba(98, 145, 162, 255), rgba(122, 177, 198, 255),
                0.3, 3
            ),
            lobby_list_bg = style(
                rgba(10, 15, 19, 235), rgba(10, 15, 19, 235), rgba(10, 15, 19, 235),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(66, 97, 112, 255), rgba(66, 97, 112, 255), rgba(66, 97, 112, 255),
                0.3, 3
            )
        },
        game_menu = {
            main_menu = style(
                rgba(20, 30, 36, 235), rgba(30, 46, 54, 245), rgba(14, 20, 24, 255),
                rgba(205, 238, 246, 255), rgba(236, 250, 255, 255), rgba(162, 210, 226, 255),
                rgba(72, 132, 150, 255), rgba(118, 196, 220, 255), rgba(152, 226, 245, 255),
                0.45, 4
            )
        },
        settings = {
            main_menu = style(
                rgba(20, 30, 36, 235), rgba(30, 46, 54, 245), rgba(14, 20, 24, 255),
                rgba(205, 238, 246, 255), rgba(236, 250, 255, 255), rgba(162, 210, 226, 255),
                rgba(72, 132, 150, 255), rgba(118, 196, 220, 255), rgba(152, 226, 245, 255),
                0.45, 4
            ),
            title = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(186, 228, 242, 255), rgba(210, 241, 252, 255), rgba(148, 196, 214, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            nav_panel = style(
                rgba(12, 18, 22, 240), rgba(12, 18, 22, 240), rgba(12, 18, 22, 240),
                rgba(190, 225, 237, 255), rgba(190, 225, 237, 255), rgba(190, 225, 237, 255),
                rgba(54, 88, 102, 255), rgba(54, 88, 102, 255), rgba(54, 88, 102, 255),
                0.15, 2
            )
        },
        difficulty = {
            main_menu = style(
                rgba(20, 30, 36, 235), rgba(30, 46, 54, 245), rgba(14, 20, 24, 255),
                rgba(205, 238, 246, 255), rgba(236, 250, 255, 255), rgba(162, 210, 226, 255),
                rgba(72, 132, 150, 255), rgba(118, 196, 220, 255), rgba(152, 226, 245, 255),
                0.45, 4
            ),
            label = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(190, 230, 242, 255), rgba(190, 230, 242, 255), rgba(190, 230, 242, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            )
        },
        game_over_lost = {
            backdrop = style(
                rgba(5, 11, 15, 225), rgba(5, 11, 15, 225), rgba(5, 11, 15, 225),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            frame = style(
                rgba(15, 22, 28, 245), rgba(15, 22, 28, 245), rgba(15, 22, 28, 245),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(225, 88, 66, 255), rgba(225, 88, 66, 255), rgba(225, 88, 66, 255),
                0.6, 4
            ),
            accent = style(
                rgba(225, 88, 66, 255), rgba(225, 88, 66, 255), rgba(225, 88, 66, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(225, 88, 66, 255), rgba(225, 88, 66, 255), rgba(225, 88, 66, 255),
                0.2, 0
            ),
            title = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(245, 222, 214, 255), rgba(245, 222, 214, 255), rgba(245, 222, 214, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            subtitle = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(206, 185, 178, 255), rgba(206, 185, 178, 255), rgba(206, 185, 178, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            stat_panel = style(
                rgba(10, 17, 22, 255), rgba(10, 17, 22, 255), rgba(10, 17, 22, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(94, 98, 106, 255), rgba(94, 98, 106, 255), rgba(94, 98, 106, 255),
                0.3, 3
            ),
            stat_label = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(166, 168, 173, 255), rgba(166, 168, 173, 255), rgba(166, 168, 173, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            stat_value = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(250, 232, 190, 255), rgba(250, 232, 190, 255), rgba(250, 232, 190, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            primary_button = style(
                rgba(216, 84, 62, 255), rgba(235, 116, 92, 255), rgba(174, 58, 42, 255),
                rgba(250, 242, 240, 255), rgba(255, 255, 255, 255), rgba(238, 216, 210, 255),
                rgba(245, 168, 140, 255), rgba(255, 196, 165, 255), rgba(235, 116, 92, 255),
                0.5, 5
            ),
            secondary_button = style(
                rgba(34, 43, 50, 255), rgba(50, 62, 74, 255), rgba(22, 28, 34, 255),
                rgba(212, 221, 228, 255), rgba(242, 246, 250, 255), rgba(186, 198, 208, 255),
                rgba(86, 104, 118, 255), rgba(130, 152, 170, 255), rgba(148, 169, 186, 255),
                0.5, 4
            ),
            dummy_button = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            )
        },
        game_over_won = {
            backdrop = style(
                rgba(6, 14, 12, 225), rgba(6, 14, 12, 225), rgba(6, 14, 12, 225),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            frame = style(
                rgba(14, 25, 22, 245), rgba(14, 25, 22, 245), rgba(14, 25, 22, 245),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(72, 188, 132, 255), rgba(72, 188, 132, 255), rgba(72, 188, 132, 255),
                0.6, 4
            ),
            accent = style(
                rgba(72, 188, 132, 255), rgba(72, 188, 132, 255), rgba(72, 188, 132, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(72, 188, 132, 255), rgba(72, 188, 132, 255), rgba(72, 188, 132, 255),
                0.2, 0
            ),
            title = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(228, 248, 236, 255), rgba(228, 248, 236, 255), rgba(228, 248, 236, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            subtitle = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(186, 218, 198, 255), rgba(186, 218, 198, 255), rgba(186, 218, 198, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            stat_panel = style(
                rgba(11, 19, 17, 255), rgba(11, 19, 17, 255), rgba(11, 19, 17, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(90, 124, 108, 255), rgba(90, 124, 108, 255), rgba(90, 124, 108, 255),
                0.3, 3
            ),
            stat_label = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(150, 178, 160, 255), rgba(150, 178, 160, 255), rgba(150, 178, 160, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            stat_value = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(220, 248, 226, 255), rgba(220, 248, 226, 255), rgba(220, 248, 226, 255),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            ),
            primary_button = style(
                rgba(60, 168, 118, 255), rgba(92, 194, 146, 255), rgba(44, 132, 92, 255),
                rgba(236, 250, 242, 255), rgba(255, 255, 255, 255), rgba(206, 234, 220, 255),
                rgba(140, 215, 175, 255), rgba(176, 233, 200, 255), rgba(92, 194, 146, 255),
                0.5, 5
            ),
            secondary_button = style(
                rgba(34, 48, 44, 255), rgba(52, 74, 66, 255), rgba(22, 33, 30, 255),
                rgba(212, 236, 220, 255), rgba(244, 250, 246, 255), rgba(182, 205, 192, 255),
                rgba(96, 122, 111, 255), rgba(132, 166, 150, 255), rgba(154, 182, 170, 255),
                0.5, 4
            ),
            dummy_button = style(
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                rgba(0, 0, 0, 0), rgba(0, 0, 0, 0), rgba(0, 0, 0, 0),
                0, 0
            )
        }
    }
}
