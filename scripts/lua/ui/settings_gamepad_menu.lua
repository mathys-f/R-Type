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

Create_ui_text("nav_panel", {
    content = "",
    font_size = 1
})

Set_ui_style("nav_panel", styles.nav_panel)

Set_ui_transform("nav_panel", {
    x = 0,
    y = 0,
    z = 0,
    w = 20.37,
    h = 100,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("nav_title", {
    content = "Settings",
    font_size = 28
})

Set_ui_style("nav_title", styles.title)

Set_ui_transform("nav_title", {
    x = 2.31,
    y = 3.47,
    z = 0,
    w = 15.74,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_controls_button")

Set_ui_style("nav_controls_button", styles.main_menu)

Set_ui_text("nav_controls_button", {
    content = "Keyboard",
    font_size = 20
})

Set_ui_transform("nav_controls_button", {
    x = 1.85,
    y = 12.5,
    z = 0,
    w = 16.67,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_gamepad_button")

Set_ui_style("nav_gamepad_button", styles.main_menu)

Set_ui_text("nav_gamepad_button", {
    content = "Controller",
    font_size = 20
})

Set_ui_transform("nav_gamepad_button", {
    x = 1.85,
    y = 21.53,
    z = 0,
    w = 16.67,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_audio_button")

Set_ui_style("nav_audio_button", styles.main_menu)

Set_ui_text("nav_audio_button", {
    content = "Audio",
    font_size = 20
})

Set_ui_transform("nav_audio_button", {
    x = 1.85,
    y = 30.56,
    z = 0,
    w = 16.67,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("settings_title", {
    content = "Settings",
    font_size = 80
})

Set_ui_style("settings_title", styles.title)

Set_ui_transform("settings_title", {
    x = 24.07,
    y = 1.39,
    z = 0,
    w = 70.37,
    h = 16.67,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_controls_title", {
    content = "Controller Controls",
    font_size = 36
})

Set_ui_style("gamepad_controls_title", styles.title)

Set_ui_transform("gamepad_controls_title", {
    x = 24.07,
    y = 22.22,
    z = 0,
    w = 55.56,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_controls_note", {
    content = "Movement also uses the left joystick in-game",
    font_size = 20
})

Set_ui_style("gamepad_controls_note", styles.title)

Set_ui_transform("gamepad_controls_note", {
    x = 24.07,
    y = 27.78,
    z = 0,
    w = 70.37,
    h = 4.17,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_mapping_move_up", {
    content = "Move Up: Dpad Up",
    font_size = 28
})

Set_ui_style("gamepad_mapping_move_up", styles.title)

Set_ui_transform("gamepad_mapping_move_up", {
    x = 24.07,
    y = 34.72,
    z = 0,
    w = 42.59,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("rebind_gamepad_move_up")

Set_ui_style("rebind_gamepad_move_up", styles.main_menu)

Set_ui_text("rebind_gamepad_move_up", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform("rebind_gamepad_move_up", {
    x = 70.37,
    y = 34.03,
    z = 0,
    w = 18.52,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_mapping_move_down", {
    content = "Move Down: Dpad Down",
    font_size = 28
})

Set_ui_style("gamepad_mapping_move_down", styles.title)

Set_ui_transform("gamepad_mapping_move_down", {
    x = 24.07,
    y = 43.06,
    z = 0,
    w = 42.59,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("rebind_gamepad_move_down")

Set_ui_style("rebind_gamepad_move_down", styles.main_menu)

Set_ui_text("rebind_gamepad_move_down", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform("rebind_gamepad_move_down", {
    x = 70.37,
    y = 42.36,
    z = 0,
    w = 18.52,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_mapping_move_left", {
    content = "Move Left: Dpad Left",
    font_size = 28
})

Set_ui_style("gamepad_mapping_move_left", styles.title)

Set_ui_transform("gamepad_mapping_move_left", {
    x = 24.07,
    y = 51.39,
    z = 0,
    w = 42.59,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("rebind_gamepad_move_left")

Set_ui_style("rebind_gamepad_move_left", styles.main_menu)

Set_ui_text("rebind_gamepad_move_left", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform("rebind_gamepad_move_left", {
    x = 70.37,
    y = 50.69,
    z = 0,
    w = 18.52,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_mapping_move_right", {
    content = "Move Right: Dpad Right",
    font_size = 28
})

Set_ui_style("gamepad_mapping_move_right", styles.title)

Set_ui_transform("gamepad_mapping_move_right", {
    x = 24.07,
    y = 59.72,
    z = 0,
    w = 42.59,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("rebind_gamepad_move_right")

Set_ui_style("rebind_gamepad_move_right", styles.main_menu)

Set_ui_text("rebind_gamepad_move_right", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform("rebind_gamepad_move_right", {
    x = 70.37,
    y = 59.03,
    z = 0,
    w = 18.52,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_mapping_shoot", {
    content = "Shoot: South",
    font_size = 28
})

Set_ui_style("gamepad_mapping_shoot", styles.title)

Set_ui_transform("gamepad_mapping_shoot", {
    x = 24.07,
    y = 68.06,
    z = 0,
    w = 42.59,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("rebind_gamepad_shoot")

Set_ui_style("rebind_gamepad_shoot", styles.main_menu)

Set_ui_text("rebind_gamepad_shoot", {
    content = "Rebind",
    font_size = 20
})

Set_ui_transform("rebind_gamepad_shoot", {
    x = 70.37,
    y = 67.36,
    z = 0,
    w = 18.52,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("gamepad_rebind_prompt", {
    content = "Click Rebind to change controls",
    font_size = 24
})

Set_ui_style("gamepad_rebind_prompt", styles.title)

Set_ui_transform("gamepad_rebind_prompt", {
    x = 24.07,
    y = 77.78,
    z = 0,
    w = 70.37,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("reset_gamepad_button")

Set_ui_style("reset_gamepad_button", styles.main_menu)

Set_ui_text("reset_gamepad_button", {
    content = "Reset",
    font_size = 20
})

Set_ui_transform("reset_gamepad_button", {
    x = 83.33,
    y = 90.28,
    z = 0,
    w = 11.11,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("back_button")

Set_ui_style("back_button", styles.main_menu)

Set_ui_text("back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform("back_button", {
    x = 24.07,
    y = 90.28,
    z = 0,
    w = 9.26,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("exit_button")

Set_ui_style("exit_button", styles.main_menu)

Set_ui_text("exit_button", {
    content = "Exit",
    font_size = 20
})

Set_ui_transform("exit_button", {
    x = 36.11,
    y = 90.28,
    z = 0,
    w = 9.26,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("main_menu_button")

Set_ui_style("main_menu_button", styles.main_menu)

Set_ui_text("main_menu_button", {
    content = "Menu",
    font_size = 20
})

Set_ui_transform("main_menu_button", {
    x = 48.15,
    y = 90.28,
    z = 0,
    w = 11.11,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation("nav_controls_button", { up = "nav_audio_button", down = "nav_gamepad_button", right = "rebind_gamepad_move_up" })
Set_ui_navigation("nav_gamepad_button", { up = "nav_controls_button", down = "nav_audio_button", right = "rebind_gamepad_move_up" })
Set_ui_navigation("nav_audio_button", { up = "nav_gamepad_button", down = "nav_controls_button", right = "rebind_gamepad_move_up" })

Set_ui_navigation("rebind_gamepad_move_up", { up = "nav_gamepad_button", down = "rebind_gamepad_move_down", left = "nav_gamepad_button" })
Set_ui_navigation("rebind_gamepad_move_down", { up = "rebind_gamepad_move_up", down = "rebind_gamepad_move_left", left = "nav_gamepad_button" })
Set_ui_navigation("rebind_gamepad_move_left", { up = "rebind_gamepad_move_down", down = "rebind_gamepad_move_right", left = "nav_gamepad_button" })
Set_ui_navigation("rebind_gamepad_move_right", { up = "rebind_gamepad_move_left", down = "rebind_gamepad_shoot", left = "nav_gamepad_button" })
Set_ui_navigation("rebind_gamepad_shoot", { up = "rebind_gamepad_move_right", down = "main_menu_button", left = "nav_gamepad_button" })
Set_ui_navigation("reset_gamepad_button", { up = "rebind_gamepad_shoot", down = "rebind_gamepad_move_up", left = "main_menu_button", right = "back_button" })

Set_ui_navigation("back_button", { up = "rebind_gamepad_shoot", left = "nav_gamepad_button", right = "exit_button", down = "rebind_gamepad_move_up" })
Set_ui_navigation("exit_button", { up = "rebind_gamepad_shoot", left = "back_button", right = "main_menu_button", down = "rebind_gamepad_move_up" })
Set_ui_navigation("main_menu_button", { up = "rebind_gamepad_shoot", left = "exit_button", right = "reset_gamepad_button", down = "rebind_gamepad_move_up" })
