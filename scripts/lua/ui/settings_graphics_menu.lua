-- Graphics Settings Menu UI
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

-- Navigation Panel
Create_ui_text("nav_panel", {
    content = "",
    font_size = 1
})
Set_ui_style("nav_panel", styles.nav_panel)
Set_ui_transform("nav_panel", {
    x = 0, y = 0, z = 0,
    w = 20.37, h = 100,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("nav_title", {
    content = "Settings",
    font_size = 28
})
Set_ui_style("nav_title", styles.title)
Set_ui_transform("nav_title", {
    x = 2.31, y = 3.47, z = 0,
    w = 15.74, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_controls_button")
Set_ui_style("nav_controls_button", styles.main_menu)
Set_ui_text("nav_controls_button", {
    content = "Keyboard",
    font_size = 20
})
Set_ui_transform("nav_controls_button", {
    x = 1.85, y = 12.5, z = 0,
    w = 16.67, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_gamepad_button")
Set_ui_style("nav_gamepad_button", styles.main_menu)
Set_ui_text("nav_gamepad_button", {
    content = "Controller",
    font_size = 20
})
Set_ui_transform("nav_gamepad_button", {
    x = 1.85, y = 21.53, z = 0,
    w = 16.67, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_audio_button")
Set_ui_style("nav_audio_button", styles.main_menu)
Set_ui_text("nav_audio_button", {
    content = "Audio",
    font_size = 20
})
Set_ui_transform("nav_audio_button", {
    x = 1.85, y = 30.56, z = 0,
    w = 16.67, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("nav_graphics_button")
Set_ui_style("nav_graphics_button", styles.main_menu)
Set_ui_text("nav_graphics_button", {
    content = "Graphics",
    font_size = 20
})
Set_ui_transform("nav_graphics_button", {
    x = 1.85, y = 39.58, z = 0,
    w = 16.67, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Main Title
Create_ui_text("settings_title", {
    content = "Settings",
    font_size = 80
})
Set_ui_style("settings_title", styles.title)
Set_ui_transform("settings_title", {
    x = 24.07, y = 1.39, z = 0,
    w = 70.37, h = 16.67,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Graphics Title
Create_ui_text("graphics_title", {
    content = "Graphics",
    font_size = 36
})
Set_ui_style("graphics_title", styles.title)
Set_ui_transform("graphics_title", {
    x = 24.07, y = 22.22, z = 0,
    w = 55.56, h = 6.94,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Scroll Speed Section
Create_ui_text("scroll_speed_label", {
    content = "Scroll Speed",
    font_size = 28
})
Set_ui_style("scroll_speed_label", styles.title)
Set_ui_transform("scroll_speed_label", {
    x = 24.07, y = 33.33, z = 0,
    w = 27.78, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("scroll_speed_value", {
    content = "5",
    font_size = 28
})
Set_ui_style("scroll_speed_value", styles.title)
Set_ui_transform("scroll_speed_value", {
    x = 55.56, y = 33.33, z = 0,
    w = 11.11, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("scroll_speed_down_button")
Set_ui_text("scroll_speed_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("scroll_speed_down_button", styles.main_menu)
Set_ui_transform("scroll_speed_down_button", {
    x = 70.37, y = 32.64, z = 0,
    w = 5.56, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("scroll_speed_up_button")
Set_ui_text("scroll_speed_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("scroll_speed_up_button", styles.main_menu)
Set_ui_transform("scroll_speed_up_button", {
    x = 76.85, y = 32.64, z = 0,
    w = 5.56, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Particles Section
Create_ui_text("particles_label", {
    content = "Particle Density",
    font_size = 28
})
Set_ui_style("particles_label", styles.title)
Set_ui_transform("particles_label", {
    x = 24.07, y = 42.36, z = 0,
    w = 27.78, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("particles_value", {
    content = "3",
    font_size = 28
})
Set_ui_style("particles_value", styles.title)
Set_ui_transform("particles_value", {
    x = 55.56, y = 42.36, z = 0,
    w = 11.11, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("particles_down_button")
Set_ui_text("particles_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("particles_down_button", styles.main_menu)
Set_ui_transform("particles_down_button", {
    x = 70.37, y = 41.67, z = 0,
    w = 5.56, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("particles_up_button")
Set_ui_text("particles_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("particles_up_button", styles.main_menu)
Set_ui_transform("particles_up_button", {
    x = 76.85, y = 41.67, z = 0,
    w = 5.56, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Stars Section
Create_ui_text("stars_label", {
    content = "Star Count",
    font_size = 28
})
Set_ui_style("stars_label", styles.title)
Set_ui_transform("stars_label", {
    x = 24.07, y = 51.39, z = 0,
    w = 27.78, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("stars_value", {
    content = "1000",
    font_size = 28
})
Set_ui_style("stars_value", styles.title)
Set_ui_transform("stars_value", {
    x = 55.56, y = 51.39, z = 0,
    w = 11.11, h = 5.56,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("stars_down_button")
Set_ui_text("stars_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("stars_down_button", styles.main_menu)
Set_ui_transform("stars_down_button", {
    x = 70.37, y = 50.69, z = 0,
    w = 5.56, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("stars_up_button")
Set_ui_text("stars_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("stars_up_button", styles.main_menu)
Set_ui_transform("stars_up_button", {
    x = 76.85, y = 50.69, z = 0,
    w = 5.56, h = 6.25,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Bottom Buttons
Create_ui_button("back_button")
Set_ui_text("back_button", {
    content = "Back",
    font_size = 20
})
Set_ui_style("back_button", styles.main_menu)
Set_ui_transform("back_button", {
    x = 24.07, y = 90.28, z = 0,
    w = 9.26, h = 6.94,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("exit_button")
Set_ui_text("exit_button", {
    content = "Exit",
    font_size = 20
})
Set_ui_style("exit_button", styles.main_menu)
Set_ui_transform("exit_button", {
    x = 36.11, y = 90.28, z = 0,
    w = 9.26, h = 6.94,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("main_menu_button")
Set_ui_text("main_menu_button", {
    content = "Menu",
    font_size = 20
})
Set_ui_style("main_menu_button", styles.main_menu)
Set_ui_transform("main_menu_button", {
    x = 48.15, y = 90.28, z = 0,
    w = 11.11, h = 6.94,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Navigation
Set_ui_navigation("nav_controls_button", { up = "nav_graphics_button", down = "nav_gamepad_button", right = "scroll_speed_down_button" })
Set_ui_navigation("nav_gamepad_button", { up = "nav_controls_button", down = "nav_audio_button", right = "scroll_speed_down_button" })
Set_ui_navigation("nav_audio_button", { up = "nav_gamepad_button", down = "nav_graphics_button", right = "scroll_speed_down_button" })
Set_ui_navigation("nav_graphics_button", { up = "nav_audio_button", down = "nav_controls_button", right = "scroll_speed_down_button" })

Set_ui_navigation("scroll_speed_down_button", { up = "nav_graphics_button", down = "particles_down_button", left = "nav_graphics_button", right = "scroll_speed_up_button" })
Set_ui_navigation("scroll_speed_up_button", { up = "nav_graphics_button", down = "particles_up_button", left = "scroll_speed_down_button", right = "scroll_speed_down_button" })

Set_ui_navigation("particles_down_button", { up = "scroll_speed_down_button", down = "stars_down_button", left = "nav_graphics_button", right = "particles_up_button" })
Set_ui_navigation("particles_up_button", { up = "scroll_speed_up_button", down = "stars_up_button", left = "particles_down_button", right = "particles_down_button" })

Set_ui_navigation("stars_down_button", { up = "particles_down_button", down = "back_button", left = "nav_graphics_button", right = "stars_up_button" })
Set_ui_navigation("stars_up_button", { up = "particles_up_button", down = "exit_button", left = "stars_down_button", right = "stars_down_button" })

Set_ui_navigation("back_button", { up = "stars_down_button", left = "nav_graphics_button", right = "exit_button" })
Set_ui_navigation("exit_button", { up = "stars_down_button", left = "back_button", right = "main_menu_button" })
Set_ui_navigation("main_menu_button", { up = "stars_down_button", left = "exit_button", right = "back_button" })
