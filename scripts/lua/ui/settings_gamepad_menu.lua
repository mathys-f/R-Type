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
    },
    nav_panel = {
        background_color =          {r = 20,  g = 20,  b = 20,  a = 255},
        background_color_hovered =  {r = 20,  g = 20,  b = 20,  a = 255},
        background_color_pressed =  {r = 20,  g = 20,  b = 20,  a = 255},
        text_color =                {r = 200, g = 200, b = 200, a = 255},
        text_color_hovered =        {r = 200, g = 200, b = 200, a = 255},
        text_color_pressed =        {r = 200, g = 200, b = 200, a = 255},
        border_color =              {r = 60, g = 60, b = 60, a = 255},
        border_color_hovered =      {r = 60, g = 60, b = 60, a = 255},
        border_color_pressed =      {r = 60, g = 60, b = 60, a = 255},
        border_radius = 0,
        border_thickness = 2
    }
}

Create_ui_text(6, "nav_panel", {
    content = "",
    font_size = 1
})

Set_ui_style(6, "nav_panel", styles.nav_panel)

Set_ui_transform(6, "nav_panel", {
    x = 0,
    y = 0,
    z = 0,
    w = 220,
    h = 720,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "nav_title", {
    content = "Settings",
    font_size = 28
})

Set_ui_style(6, "nav_title", styles.title)

Set_ui_transform(6, "nav_title", {
    x = 25,
    y = 25,
    z = 0,
    w = 170,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "nav_controls_button")

Set_ui_style(6, "nav_controls_button", styles.main_menu)

Set_ui_text(6, "nav_controls_button", {
    content = "Keyboard",
    font_size = 20
})

Set_ui_transform(6, "nav_controls_button", {
    x = 20,
    y = 90,
    z = 0,
    w = 180,
    h = 45,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "nav_gamepad_button")

Set_ui_style(6, "nav_gamepad_button", styles.main_menu)

Set_ui_text(6, "nav_gamepad_button", {
    content = "Controller",
    font_size = 20
})

Set_ui_transform(6, "nav_gamepad_button", {
    x = 20,
    y = 155,
    z = 0,
    w = 180,
    h = 45,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "nav_audio_button")

Set_ui_style(6, "nav_audio_button", styles.main_menu)

Set_ui_text(6, "nav_audio_button", {
    content = "Audio",
    font_size = 20
})

Set_ui_transform(6, "nav_audio_button", {
    x = 20,
    y = 220,
    z = 0,
    w = 180,
    h = 45,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "settings_title", {
    content = "Settings",
    font_size = 80
})

Set_ui_style(6, "settings_title", styles.title)

Set_ui_transform(6, "settings_title", {
    x = 260,
    y = 10,
    z = 0,
    w = 760,
    h = 120,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_controls_title", {
    content = "Controller Controls",
    font_size = 36
})

Set_ui_style(6, "gamepad_controls_title", styles.title)

Set_ui_transform(6, "gamepad_controls_title", {
    x = 260,
    y = 160,
    z = 0,
    w = 600,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_controls_note", {
    content = "Movement also uses the left joystick in-game",
    font_size = 20
})

Set_ui_style(6, "gamepad_controls_note", styles.title)

Set_ui_transform(6, "gamepad_controls_note", {
    x = 260,
    y = 200,
    z = 0,
    w = 760,
    h = 30,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_mapping_move_up", {
    content = "Move Up: Dpad Up",
    font_size = 28
})

Set_ui_style(6, "gamepad_mapping_move_up", styles.title)

Set_ui_transform(6, "gamepad_mapping_move_up", {
    x = 260,
    y = 250,
    z = 0,
    w = 460,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "rebind_gamepad_move_up")

Set_ui_style(6, "rebind_gamepad_move_up", styles.main_menu)

Set_ui_text(6, "rebind_gamepad_move_up", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(6, "rebind_gamepad_move_up", {
    x = 760,
    y = 245,
    z = 0,
    w = 200,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_mapping_move_down", {
    content = "Move Down: Dpad Down",
    font_size = 28
})

Set_ui_style(6, "gamepad_mapping_move_down", styles.title)

Set_ui_transform(6, "gamepad_mapping_move_down", {
    x = 260,
    y = 310,
    z = 0,
    w = 460,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "rebind_gamepad_move_down")

Set_ui_style(6, "rebind_gamepad_move_down", styles.main_menu)

Set_ui_text(6, "rebind_gamepad_move_down", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(6, "rebind_gamepad_move_down", {
    x = 760,
    y = 305,
    z = 0,
    w = 200,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_mapping_move_left", {
    content = "Move Left: Dpad Left",
    font_size = 28
})

Set_ui_style(6, "gamepad_mapping_move_left", styles.title)

Set_ui_transform(6, "gamepad_mapping_move_left", {
    x = 260,
    y = 370,
    z = 0,
    w = 460,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "rebind_gamepad_move_left")

Set_ui_style(6, "rebind_gamepad_move_left", styles.main_menu)

Set_ui_text(6, "rebind_gamepad_move_left", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(6, "rebind_gamepad_move_left", {
    x = 760,
    y = 365,
    z = 0,
    w = 200,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_mapping_move_right", {
    content = "Move Right: Dpad Right",
    font_size = 28
})

Set_ui_style(6, "gamepad_mapping_move_right", styles.title)

Set_ui_transform(6, "gamepad_mapping_move_right", {
    x = 260,
    y = 430,
    z = 0,
    w = 460,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "rebind_gamepad_move_right")

Set_ui_style(6, "rebind_gamepad_move_right", styles.main_menu)

Set_ui_text(6, "rebind_gamepad_move_right", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(6, "rebind_gamepad_move_right", {
    x = 760,
    y = 425,
    z = 0,
    w = 200,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_mapping_shoot", {
    content = "Shoot: South",
    font_size = 28
})

Set_ui_style(6, "gamepad_mapping_shoot", styles.title)

Set_ui_transform(6, "gamepad_mapping_shoot", {
    x = 260,
    y = 490,
    z = 0,
    w = 460,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "rebind_gamepad_shoot")

Set_ui_style(6, "rebind_gamepad_shoot", styles.main_menu)

Set_ui_text(6, "rebind_gamepad_shoot", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform(6, "rebind_gamepad_shoot", {
    x = 760,
    y = 485,
    z = 0,
    w = 200,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(6, "gamepad_rebind_prompt", {
    content = "Click Rebind to change controls",
    font_size = 24
})

Set_ui_style(6, "gamepad_rebind_prompt", styles.title)

Set_ui_transform(6, "gamepad_rebind_prompt", {
    x = 260,
    y = 560,
    z = 0,
    w = 760,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "reset_gamepad_button")

Set_ui_style(6, "reset_gamepad_button", styles.main_menu)

Set_ui_text(6, "reset_gamepad_button", {
    content = "Reset",
    font_size = 20
})

Set_ui_transform(6, "reset_gamepad_button", {
    x = 900,
    y = 650,
    z = 0,
    w = 120,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "back_button")

Set_ui_style(6, "back_button", styles.main_menu)

Set_ui_text(6, "back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform(6, "back_button", {
    x = 260,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "exit_button")

Set_ui_style(6, "exit_button", styles.main_menu)

Set_ui_text(6, "exit_button", {
    content = "Exit",
    font_size = 20
})

Set_ui_transform(6, "exit_button", {
    x = 390,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(6, "main_menu_button")

Set_ui_style(6, "main_menu_button", styles.main_menu)

Set_ui_text(6, "main_menu_button", {
    content = "Menu",
    font_size = 20
})

Set_ui_transform(6, "main_menu_button", {
    x = 520,
    y = 650,
    z = 0,
    w = 120,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation(6, "nav_controls_button", { up = "nav_audio_button", down = "nav_gamepad_button", right = "rebind_gamepad_move_up" })
Set_ui_navigation(6, "nav_gamepad_button", { up = "nav_controls_button", down = "nav_audio_button", right = "rebind_gamepad_move_up" })
Set_ui_navigation(6, "nav_audio_button", { up = "nav_gamepad_button", down = "nav_controls_button", right = "rebind_gamepad_move_up" })

Set_ui_navigation(6, "rebind_gamepad_move_up", { up = "nav_gamepad_button", down = "rebind_gamepad_move_down", left = "nav_gamepad_button" })
Set_ui_navigation(6, "rebind_gamepad_move_down", { up = "rebind_gamepad_move_up", down = "rebind_gamepad_move_left", left = "nav_gamepad_button" })
Set_ui_navigation(6, "rebind_gamepad_move_left", { up = "rebind_gamepad_move_down", down = "rebind_gamepad_move_right", left = "nav_gamepad_button" })
Set_ui_navigation(6, "rebind_gamepad_move_right", { up = "rebind_gamepad_move_left", down = "rebind_gamepad_shoot", left = "nav_gamepad_button" })
Set_ui_navigation(6, "rebind_gamepad_shoot", { up = "rebind_gamepad_move_right", down = "main_menu_button", left = "nav_gamepad_button" })
Set_ui_navigation(6, "reset_gamepad_button", { up = "rebind_gamepad_shoot", down = "rebind_gamepad_move_up", left = "main_menu_button", right = "back_button" })

Set_ui_navigation(6, "back_button", { up = "rebind_gamepad_shoot", left = "nav_gamepad_button", right = "exit_button", down = "rebind_gamepad_move_up" })
Set_ui_navigation(6, "exit_button", { up = "rebind_gamepad_shoot", left = "back_button", right = "main_menu_button", down = "rebind_gamepad_move_up" })
Set_ui_navigation(6, "main_menu_button", { up = "rebind_gamepad_shoot", left = "exit_button", right = "reset_gamepad_button", down = "rebind_gamepad_move_up" })
