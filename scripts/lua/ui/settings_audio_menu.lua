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
    w = 220,
    h = 720,
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
    x = 25,
    y = 25,
    z = 0,
    w = 170,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(5, "nav_controls_button")

Set_ui_style(5, "nav_controls_button", styles.main_menu)

Set_ui_text(5, "nav_controls_button", {
    content = "Controls",
    font_size = 20
})

Set_ui_transform(5, "nav_controls_button", {
    x = 20,
    y = 90,
    z = 0,
    w = 180,
    h = 45,
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
    x = 20,
    y = 155,
    z = 0,
    w = 180,
    h = 45,
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
    x = 260,
    y = 10,
    z = 0,
    w = 760,
    h = 120,
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
    x = 260,
    y = 160,
    z = 0,
    w = 600,
    h = 50,
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
    x = 260,
    y = 240,
    z = 0,
    w = 300,
    h = 40,
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
    x = 600,
    y = 240,
    z = 0,
    w = 120,
    h = 40,
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
    x = 760,
    y = 235,
    z = 0,
    w = 60,
    h = 45,
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
    x = 830,
    y = 235,
    z = 0,
    w = 60,
    h = 45,
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
    x = 900,
    y = 235,
    z = 0,
    w = 100,
    h = 45,
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
    x = 260,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
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
    x = 390,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
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
    x = 520,
    y = 650,
    z = 0,
    w = 120,
    h = 50,
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
    x = 260,
    y = 305,
    z = 0,
    w = 300,
    h = 40,
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
    x = 600,
    y = 305,
    z = 0,
    w = 120,
    h = 40,
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
    x = 760,
    y = 300,
    z = 0,
    w = 60,
    h = 45,
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
    x = 830,
    y = 300,
    z = 0,
    w = 60,
    h = 45,
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
    x = 900,
    y = 300,
    z = 0,
    w = 100,
    h = 45,
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
    x = 260,
    y = 370,
    z = 0,
    w = 300,
    h = 40,
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
    x = 600,
    y = 370,
    z = 0,
    w = 120,
    h = 40,
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
    x = 760,
    y = 365,
    z = 0,
    w = 60,
    h = 45,
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
    x = 830,
    y = 365,
    z = 0,
    w = 60,
    h = 45,
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
    x = 900,
    y = 365,
    z = 0,
    w = 100,
    h = 45,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
