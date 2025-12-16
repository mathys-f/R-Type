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
    border_thickness = 5
}

---@type UIStyle
local titlescreen_style = {
    background_color =          {r = 0,  g = 0,  b = 0,  a = 0},
    background_color_hovered =  {r = 0,  g = 0,  b = 0,  a = 0},
    background_color_pressed =  {r = 0,  g = 0,  b = 0,  a = 0},
    text_color =                {r = 200, g = 200, b = 200, a = 255},
    text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
    text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
    border_color =              {r = 0, g = 0, b = 0, a = 0},
    border_color_hovered =      {r = 0, g = 0, b = 0, a = 0},
    border_color_pressed =      {r = 0, g = 0, b = 0, a = 0},
    border_radius = 0,
    border_thickness = 0
}

Create_ui_text(1, "titlescreen", {
    content = "R-Type",
    font_size = 120
})

Set_ui_style(1, "titlescreen", titlescreen_style)

Set_ui_transform(1, "titlescreen", {
    x = 0,
    y = 25,
    z = 0,
    w = 750,
    h = 150,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(1, "play_solo_button")

Set_ui_text(1, "play_solo_button", {
    content = "Play",
    font_size = 32
})

Set_ui_style(1, "play_solo_button", main_menu_style)

Set_ui_transform(1, "play_solo_button", {
    x = 300,
    y = 250,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(1, "play_multiplayer_button")

Set_ui_text(1, "play_multiplayer_button", {
    content = "Multiplayer",
    font_size = 32
})


Set_ui_style(1, "play_multiplayer_button", main_menu_style)

Set_ui_transform(1, "play_multiplayer_button", {
    x = 300,
    y = 325,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(1, "setting_button")

Set_ui_text(1, "setting_button", {
    content = "Settings",
    font_size = 32
})


Set_ui_style(1, "setting_button", main_menu_style)

Set_ui_transform(1, "setting_button", {
    x = 300,
    y = 400,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(1, "exit_button")

Set_ui_text(1, "exit_button", {
    content = "Exit",
    font_size = 32
})

Set_ui_style(1, "exit_button", main_menu_style)

Set_ui_transform(1, "exit_button", {
    x = 300,
    y = 475,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
