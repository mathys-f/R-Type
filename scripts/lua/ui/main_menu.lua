---@type UIStyle
local main_menu_style = {
    background_color =          {r = 50,  g = 50,  b = 50,  a = 255},
    background_color_hovered =  {r = 70,  g = 70,  b = 70,  a = 255},
    background_color_pressed =  {r = 30,  g = 30,  b = 30,  a = 255},
    text_color =                {r = 255, g = 255, b = 255, a = 255},
    text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
    text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
    border_color =              {r = 100, g = 100, b = 100, a = 255},
    border_color_hovered =      {r = 150, g = 150, b = 150, a = 255},
    border_color_pressed =      {r = 200, g = 200, b = 200, a = 255},
    border_radius = 0.5,
    border_thickness = 1
}

Create_ui_button("play_solo_button")

Set_ui_style("play_solo_button", main_menu_style)
