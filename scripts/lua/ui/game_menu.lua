-- Define styles inline since require() is not available in this Lua environment
local styles = {
    main_menu = {
        background_color =          {r = 35,  g = 35,  b = 35,  a = 255},
        background_color_hovered =  {r = 70,  g = 70,  b = 70,  a = 255},
        background_color_pressed =  {r = 30,  g = 30,  b = 30,  a = 255},
        text_color =                {r = 200, g = 200, b = 200, a = 255},
        text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
        text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
        border_color =              {r = 100, g = 100, b = 100, a = 255},
        border_color_hovered =      {r = 150, g = 150, b = 150, a = 255},
        border_color_pressed =      {r = 200, g = 200, b = 200, a = 255},
        border_radius = 0.5,
        border_thickness = 5
    }
}

Create_ui_button(0, "back_button")

Set_ui_style(0, "back_button", styles.main_menu)

Set_ui_text(0, "back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform(0, "back_button", {
    x = 2.31,
    y = 90.28,
    z = 0,
    w = 9.26,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})