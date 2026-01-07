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
    },
    title = {
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
}

Create_ui_text(4, "settings_title", {
    content = "Settings",
    font_size = 80
})

Set_ui_style(4, "settings_title", styles.title)

Set_ui_transform(4, "settings_title", {
    x = 60,
    y = 10,
    z = 0,
    w = 700,
    h = 120,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "controls_title", {
    content = "Controls",
    font_size = 36
})

Set_ui_style(4, "controls_title", styles.title)

Set_ui_transform(4, "controls_title", {
    x = 80,
    y = 160,
    z = 0,
    w = 600,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "mapping_move_up", {
    content = "Move Up: W / Up",
    font_size = 28
})

Set_ui_style(4, "mapping_move_up", styles.title)

Set_ui_transform(4, "mapping_move_up", {
    x = 100,
    y = 230,
    z = 0,
    w = 520,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "rebind_move_up")

Set_ui_style(4, "rebind_move_up", styles.main_menu)

Set_ui_text(4, "rebind_move_up", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(4, "rebind_move_up", {
    x = 700,
    y = 225,
    z = 0,
    w = 180,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "mapping_move_down", {
    content = "Move Down: S / Down",
    font_size = 28
})

Set_ui_style(4, "mapping_move_down", styles.title)

Set_ui_transform(4, "mapping_move_down", {
    x = 100,
    y = 290,
    z = 0,
    w = 520,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "rebind_move_down")

Set_ui_style(4, "rebind_move_down", styles.main_menu)

Set_ui_text(4, "rebind_move_down", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(4, "rebind_move_down", {
    x = 700,
    y = 285,
    z = 0,
    w = 180,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "mapping_move_left", {
    content = "Move Left: A / Left",
    font_size = 28
})

Set_ui_style(4, "mapping_move_left", styles.title)

Set_ui_transform(4, "mapping_move_left", {
    x = 100,
    y = 350,
    z = 0,
    w = 520,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "rebind_move_left")

Set_ui_style(4, "rebind_move_left", styles.main_menu)

Set_ui_text(4, "rebind_move_left", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(4, "rebind_move_left", {
    x = 700,
    y = 345,
    z = 0,
    w = 180,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "mapping_move_right", {
    content = "Move Right: D / Right",
    font_size = 28
})

Set_ui_style(4, "mapping_move_right", styles.title)

Set_ui_transform(4, "mapping_move_right", {
    x = 100,
    y = 410,
    z = 0,
    w = 520,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "rebind_move_right")

Set_ui_style(4, "rebind_move_right", styles.main_menu)

Set_ui_text(4, "rebind_move_right", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(4, "rebind_move_right", {
    x = 700,
    y = 405,
    z = 0,
    w = 180,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "mapping_shoot", {
    content = "Shoot: Space",
    font_size = 28
})

Set_ui_style(4, "mapping_shoot", styles.title)

Set_ui_transform(4, "mapping_shoot", {
    x = 100,
    y = 470,
    z = 0,
    w = 520,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "rebind_shoot")

Set_ui_style(4, "rebind_shoot", styles.main_menu)

Set_ui_text(4, "rebind_shoot", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(4, "rebind_shoot", {
    x = 700,
    y = 465,
    z = 0,
    w = 180,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(4, "rebind_prompt", {
    content = "Click Rebind to change controls",
    font_size = 24
})

Set_ui_style(4, "rebind_prompt", styles.title)

Set_ui_transform(4, "rebind_prompt", {
    x = 100,
    y = 540,
    z = 0,
    w = 780,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "back_button")

Set_ui_style(4, "back_button", styles.main_menu)

Set_ui_text(4, "back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform(4, "back_button", {
    x = 25,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "exit_button")

Set_ui_style(4, "exit_button", styles.main_menu)

Set_ui_text(4, "exit_button", {
    content = "Exit",
    font_size = 20
})

Set_ui_transform(4, "exit_button", {
    x = 155,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(4, "main_menu_button")

Set_ui_style(4, "main_menu_button", styles.main_menu)

Set_ui_text(4, "main_menu_button", {
    content = "Menu",
    font_size = 20
})

Set_ui_transform(4, "main_menu_button", {
    x = 285,
    y = 650,
    z = 0,
    w = 120,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
