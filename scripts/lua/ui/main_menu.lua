-- Define styles inline since require() is not available in this Lua environment
local styles = {
    main_menu = {
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
    },
    titlescreen = {
        background_color =          {r = 0,  g = 0,  b = 0,  a = 0},
        background_color_hovered =  {r = 0,  g = 0,  b = 0,  a = 0},
        background_color_pressed =  {r = 0,  g = 0,  b = 0,  a = 0},
        text_color =                {r = 200, g = 200, b = 200, a = 255},
        text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
        text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
        border_color =              {r = 0, g = 0, b = 0, a = 0},
        border_color_hovered =      {r = 0,  g = 0,  b = 0,  a = 0},
        border_color_pressed =      {r = 0,  g = 0,  b = 0,  a = 0},
        border_radius = 0,
        border_thickness = 0
    }
}

Create_ui_text("titlescreen", {
    content = "FTL-Type",
    font_size = 120
})

Set_ui_style("titlescreen", styles.titlescreen)

Set_ui_transform("titlescreen", {
    x = 0,
    y = 8,
    z = 0,
    w = 100,
    h = 15,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("play_solo_button")

Set_ui_text("play_solo_button", {
    content = "Play",
    font_size = 32
})

Set_ui_style("play_solo_button", styles.main_menu)

Set_ui_transform("play_solo_button", {
    x = 39.35,
    y = 34.72,
    z = 0,
    w = 18.52,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("play_multiplayer_button")

Set_ui_text("play_multiplayer_button", {
    content = "Multiplayer",
    font_size = 32
})


Set_ui_style("play_multiplayer_button", styles.main_menu)

Set_ui_transform("play_multiplayer_button", {
    x = 39.35,
    y = 45.14,
    z = 0,
    w = 18.52,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("setting_button")

Set_ui_text("setting_button", {
    content = "Settings",
    font_size = 32
})


Set_ui_style("setting_button", styles.main_menu)

Set_ui_transform("setting_button", {
    x = 39.35,
    y = 55.5,
    z = 0,
    w = 18.52,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("difficulty_button")

Set_ui_text("difficulty_button", {
    content = "Difficulty",
    font_size = 32
})

Set_ui_style("difficulty_button", styles.main_menu)

Set_ui_transform("difficulty_button", {
    x = 39.35,
    y = 65.97,
    z = 0,
    w = 18.52,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("exit_button")

Set_ui_text("exit_button", {
    content = "Exit",
    font_size = 32
})

Set_ui_style("exit_button", styles.main_menu)

Set_ui_transform("exit_button", {
    x = 39.35,
    y = 76.39,
    z = 0,
    w = 18.52,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation("play_solo_button", { up = "exit_button", down = "play_multiplayer_button" })
Set_ui_navigation("play_multiplayer_button", { up = "play_solo_button", down = "setting_button" })
Set_ui_navigation("setting_button", { up = "play_multiplayer_button", down = "difficulty_button" })
Set_ui_navigation("difficulty_button", { up = "setting_button", down = "exit_button" })
Set_ui_navigation("exit_button", { up = "difficulty_button", down = "play_solo_button" })
