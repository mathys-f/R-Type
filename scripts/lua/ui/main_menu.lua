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
    text_color =                {r = 0, g = 0, b = 0, a = 255},
    text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
    text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
    border_color =              {r = 0, g = 0, b = 0, a = 0},
    border_color_hovered =      {r = 0, g = 0, b = 0, a = 0},
    border_color_pressed =      {r = 0, g = 0, b = 0, a = 0},
    border_radius = 0,
    border_thickness = 0
}

Create_ui_text("titlescreen", {
    content = "R-Type",
    font_size = 120
})

Set_ui_style("titlescreen", titlescreen_style)

Set_ui_transform("titlescreen", {
    x = 0,
    y = 25,
    z = 0,
    w = 750,
    h = 150,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("play_solo_button")

Set_ui_text("play_solo_button", {
    content = "Play",
    font_size = 32
})

Set_ui_style("play_solo_button", main_menu_style)

Set_ui_transform("play_solo_button", {
    x = 300,
    y = 250,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("play_multiplayer_button")

Set_ui_text("play_multiplayer_button", {
    content = "Multiplayer",
    font_size = 32
})


Set_ui_style("play_multiplayer_button", main_menu_style)

Set_ui_transform("play_multiplayer_button", {
    x = 300,
    y = 325,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("setting_button")

Set_ui_text("setting_button", {
    content = "Settings",
    font_size = 32
})


Set_ui_style("setting_button", main_menu_style)

Set_ui_transform("setting_button", {
    x = 300,
    y = 400,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("exit_button")

Set_ui_text("exit_button", {
    content = "Exit",
    font_size = 32
})

Set_ui_style("exit_button", main_menu_style)

Set_ui_transform("exit_button", {
    x = 300,
    y = 475,
    z = 0,
    w = 200,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
