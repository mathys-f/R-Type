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

Create_ui_text(5, "nav_panel", {
    content = "",
    font_size = 1
})

Set_ui_style(5, "nav_panel", styles.nav_panel)

Set_ui_transform(5, "nav_panel", {
    x = 0,
    y = 0,
    z = 0,
    w = 20.37,
    h = 100,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "nav_title", {
    content = "Settings",
    font_size = 28
})

Set_ui_style(5, "nav_title", styles.title)

Set_ui_transform(5, "nav_title", {
    x = 2.31,
    y = 3.47,
    z = 0,
    w = 15.74,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "nav_controls_button")

Set_ui_style(5, "nav_controls_button", styles.main_menu)

Set_ui_text(5, "nav_controls_button", {
    content = "Keyboard",
    font_size = 20
})

Set_ui_transform(5, "nav_controls_button", {
    x = 1.85,
    y = 12.5,
    z = 0,
    w = 16.67,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "nav_audio_button")

Set_ui_style(5, "nav_audio_button", styles.main_menu)

Set_ui_text(5, "nav_audio_button", {
    content = "Audio",
    font_size = 20
})

Set_ui_transform(5, "nav_audio_button", {
    x = 1.85,
    y = 30.56,
    z = 0,
    w = 16.67,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "nav_gamepad_button")

Set_ui_style(5, "nav_gamepad_button", styles.main_menu)

Set_ui_text(5, "nav_gamepad_button", {
    content = "Controller",
    font_size = 20
})

Set_ui_transform(5, "nav_gamepad_button", {
    x = 1.85,
    y = 21.53,
    z = 0,
    w = 16.67,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "settings_title", {
    content = "Settings",
    font_size = 80
})

Set_ui_style(5, "settings_title", styles.title)

Set_ui_transform(5, "settings_title", {
    x = 24.07,
    y = 1.39,
    z = 0,
    w = 70.37,
    h = 16.67,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "audio_title", {
    content = "Audio",
    font_size = 36
})

Set_ui_style(5, "audio_title", styles.title)

Set_ui_transform(5, "audio_title", {
    x = 24.07,
    y = 22.22,
    z = 0,
    w = 55.56,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "general_volume_label", {
    content = "General Volume",
    font_size = 28
})

Set_ui_style(5, "general_volume_label", styles.title)

Set_ui_transform(5, "general_volume_label", {
    x = 24.07,
    y = 33.33,
    z = 0,
    w = 27.78,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "general_volume_value", {
    content = "100%",
    font_size = 28
})

Set_ui_style(5, "general_volume_value", styles.title)

Set_ui_transform(5, "general_volume_value", {
    x = 55.56,
    y = 33.33,
    z = 0,
    w = 11.11,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "general_volume_down_button")

Set_ui_style(5, "general_volume_down_button", styles.main_menu)

Set_ui_text(5, "general_volume_down_button", {
    content = "-",
    font_size = 24
})

Set_ui_transform(5, "general_volume_down_button", {
    x = 70.37,
    y = 32.64,
    z = 0,
    w = 5.56,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "general_volume_up_button")

Set_ui_style(5, "general_volume_up_button", styles.main_menu)

Set_ui_text(5, "general_volume_up_button", {
    content = "+",
    font_size = 24
})

Set_ui_transform(5, "general_volume_up_button", {
    x = 76.85,
    y = 32.64,
    z = 0,
    w = 5.56,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "general_mute_button")

Set_ui_style(5, "general_mute_button", styles.main_menu)

Set_ui_text(5, "general_mute_button", {
    content = "Mute",
    font_size = 18
})

Set_ui_transform(5, "general_mute_button", {
    x = 83.33,
    y = 32.64,
    z = 0,
    w = 9.26,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "back_button")

Set_ui_style(5, "back_button", styles.main_menu)

Set_ui_text(5, "back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform(5, "back_button", {
    x = 24.07,
    y = 90.28,
    z = 0,
    w = 9.26,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "exit_button")

Set_ui_style(5, "exit_button", styles.main_menu)

Set_ui_text(5, "exit_button", {
    content = "Exit",
    font_size = 20
})

Set_ui_transform(5, "exit_button", {
    x = 36.11,
    y = 90.28,
    z = 0,
    w = 9.26,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "main_menu_button")

Set_ui_style(5, "main_menu_button", styles.main_menu)

Set_ui_text(5, "main_menu_button", {
    content = "Menu",
    font_size = 20
})

Set_ui_transform(5, "main_menu_button", {
    x = 48.15,
    y = 90.28,
    z = 0,
    w = 11.11,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
Create_ui_text(5, "music_volume_label", {
    content = "Music Volume",
    font_size = 28
})

Set_ui_style(5, "music_volume_label", styles.title)

Set_ui_transform(5, "music_volume_label", {
    x = 24.07,
    y = 42.36,
    z = 0,
    w = 27.78,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "music_volume_value", {
    content = "100%",
    font_size = 28
})

Set_ui_style(5, "music_volume_value", styles.title)

Set_ui_transform(5, "music_volume_value", {
    x = 55.56,
    y = 42.36,
    z = 0,
    w = 11.11,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "music_volume_down_button")

Set_ui_style(5, "music_volume_down_button", styles.main_menu)

Set_ui_text(5, "music_volume_down_button", {
    content = "-",
    font_size = 24
})

Set_ui_transform(5, "music_volume_down_button", {
    x = 70.37,
    y = 41.67,
    z = 0,
    w = 5.56,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "music_volume_up_button")

Set_ui_style(5, "music_volume_up_button", styles.main_menu)

Set_ui_text(5, "music_volume_up_button", {
    content = "+",
    font_size = 24
})

Set_ui_transform(5, "music_volume_up_button", {
    x = 76.85,
    y = 41.67,
    z = 0,
    w = 5.56,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "music_mute_button")

Set_ui_style(5, "music_mute_button", styles.main_menu)

Set_ui_text(5, "music_mute_button", {
    content = "Mute",
    font_size = 18
})

Set_ui_transform(5, "music_mute_button", {
    x = 83.33,
    y = 41.67,
    z = 0,
    w = 9.26,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "sfx_volume_label", {
    content = "SFX Volume",
    font_size = 28
})

Set_ui_style(5, "sfx_volume_label", styles.title)

Set_ui_transform(5, "sfx_volume_label", {
    x = 24.07,
    y = 51.39,
    z = 0,
    w = 27.78,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text(5, "sfx_volume_value", {
    content = "100%",
    font_size = 28
})

Set_ui_style(5, "sfx_volume_value", styles.title)

Set_ui_transform(5, "sfx_volume_value", {
    x = 55.56,
    y = 51.39,
    z = 0,
    w = 11.11,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "sfx_volume_down_button")

Set_ui_style(5, "sfx_volume_down_button", styles.main_menu)

Set_ui_text(5, "sfx_volume_down_button", {
    content = "-",
    font_size = 24
})

Set_ui_transform(5, "sfx_volume_down_button", {
    x = 70.37,
    y = 50.69,
    z = 0,
    w = 5.56,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "sfx_volume_up_button")

Set_ui_style(5, "sfx_volume_up_button", styles.main_menu)

Set_ui_text(5, "sfx_volume_up_button", {
    content = "+",
    font_size = 24
})

Set_ui_transform(5, "sfx_volume_up_button", {
    x = 76.85,
    y = 50.69,
    z = 0,
    w = 5.56,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "sfx_mute_button")

Set_ui_style(5, "sfx_mute_button", styles.main_menu)

Set_ui_text(5, "sfx_mute_button", {
    content = "Mute",
    font_size = 18
})

Set_ui_transform(5, "sfx_mute_button", {
    x = 83.33,
    y = 50.69,
    z = 0,
    w = 9.26,
    h = 6.25,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation(5, "nav_controls_button", { up = "nav_audio_button", down = "nav_gamepad_button", right = "general_volume_down_button" })
Set_ui_navigation(5, "nav_gamepad_button", { up = "nav_controls_button", down = "nav_audio_button", right = "general_volume_down_button" })
Set_ui_navigation(5, "nav_audio_button", { up = "nav_gamepad_button", down = "nav_controls_button", right = "general_volume_down_button" })

Set_ui_navigation(5, "general_volume_down_button", { up = "nav_audio_button", down = "music_volume_down_button", left = "nav_audio_button", right = "general_volume_up_button" })
Set_ui_navigation(5, "general_volume_up_button", { up = "nav_audio_button", down = "music_volume_up_button", left = "general_volume_down_button", right = "general_mute_button" })
Set_ui_navigation(5, "general_mute_button", { up = "nav_audio_button", down = "music_mute_button", left = "general_volume_up_button", right = "general_volume_down_button" })

Set_ui_navigation(5, "music_volume_down_button", { up = "general_volume_down_button", down = "sfx_volume_down_button", left = "nav_audio_button", right = "music_volume_up_button" })
Set_ui_navigation(5, "music_volume_up_button", { up = "general_volume_up_button", down = "sfx_volume_up_button", left = "music_volume_down_button", right = "music_mute_button" })
Set_ui_navigation(5, "music_mute_button", { up = "general_mute_button", down = "sfx_mute_button", left = "music_volume_up_button", right = "music_volume_down_button" })

Set_ui_navigation(5, "sfx_volume_down_button", { up = "music_volume_down_button", down = "main_menu_button", left = "nav_audio_button", right = "sfx_volume_up_button" })
Set_ui_navigation(5, "sfx_volume_up_button", { up = "music_volume_up_button", down = "main_menu_button", left = "sfx_volume_down_button", right = "sfx_mute_button" })
Set_ui_navigation(5, "sfx_mute_button", { up = "music_mute_button", down = "main_menu_button", left = "sfx_volume_up_button", right = "sfx_volume_down_button" })
Set_ui_navigation(5, "back_button", { up = "sfx_volume_down_button", left = "nav_audio_button", right = "exit_button" })
Set_ui_navigation(5, "exit_button", { up = "sfx_volume_down_button", left = "back_button", right = "main_menu_button" })
Set_ui_navigation(5, "main_menu_button", { up = "sfx_volume_down_button", left = "exit_button", right = "back_button" })
