-- Difficulty Settings Menu UI
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
    label = {
        background_color =          {r = 0,  g = 0,  b = 0,  a = 0},
        background_color_hovered =  {r = 0,  g = 0,  b = 0,  a = 0},
        background_color_pressed =  {r = 0,  g = 0,  b = 0,  a = 0},
        text_color =                {r = 200, g = 200, b = 200, a = 255},
        text_color_hovered =        {r = 200, g = 200, b = 200, a = 255},
        text_color_pressed =        {r = 200, g = 200, b = 200, a = 255},
        border_color =              {r = 0, g = 0, b = 0, a = 0},
        border_color_hovered =      {r = 0,  g = 0,  b = 0,  a = 0},
        border_color_pressed =      {r = 0,  g = 0,  b = 0,  a = 0},
        border_radius = 0,
        border_thickness = 0
    }
}

-- Title
Create_ui_text("difficulty_title", {
    content = "Difficulty Settings",
    font_size = 48
})
Set_ui_style("difficulty_title", styles.label)
Set_ui_transform("difficulty_title", {
    x = 0, y = 5, z = 0,
    w = 100, h = 8,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Max Enemies
Create_ui_text("max_shooter_label", {
    content = "Max Flying Enemies:",
    font_size = 20
})
Set_ui_style("max_shooter_label", styles.label)
Set_ui_transform("max_shooter_label", {
    x = 20, y = 20, z = 0,
    w = 25, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("max_shooter_down_button")
Set_ui_text("max_shooter_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("max_shooter_down_button", styles.main_menu)
Set_ui_transform("max_shooter_down_button", {
    x = 50, y = 20, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("max_shooter_value", {
    content = "2",
    font_size = 20
})
Set_ui_style("max_shooter_value", styles.label)
Set_ui_transform("max_shooter_value", {
    x = 56, y = 20, z = 0,
    w = 6, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("max_shooter_up_button")
Set_ui_text("max_shooter_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("max_shooter_up_button", styles.main_menu)
Set_ui_transform("max_shooter_up_button", {
    x = 63, y = 20, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Enemy Health
Create_ui_text("enemy_health_label", {
    content = "Enemy Health:",
    font_size = 20
})
Set_ui_style("enemy_health_label", styles.label)
Set_ui_transform("enemy_health_label", {
    x = 20, y = 30, z = 0,
    w = 25, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("enemy_health_down_button")
Set_ui_text("enemy_health_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("enemy_health_down_button", styles.main_menu)
Set_ui_transform("enemy_health_down_button", {
    x = 50, y = 30, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("enemy_health_value", {
    content = "3",
    font_size = 20
})
Set_ui_style("enemy_health_value", styles.label)
Set_ui_transform("enemy_health_value", {
    x = 56, y = 30, z = 0,
    w = 6, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("enemy_health_up_button")
Set_ui_text("enemy_health_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("enemy_health_up_button", styles.main_menu)
Set_ui_transform("enemy_health_up_button", {
    x = 63, y = 30, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Enemy Speed
Create_ui_text("enemy_speed_label", {
    content = "Enemy Base Speed:",
    font_size = 20
})
Set_ui_style("enemy_speed_label", styles.label)
Set_ui_transform("enemy_speed_label", {
    x = 20, y = 40, z = 0,
    w = 25, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("enemy_speed_down_button")
Set_ui_text("enemy_speed_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("enemy_speed_down_button", styles.main_menu)
Set_ui_transform("enemy_speed_down_button", {
    x = 50, y = 40, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("enemy_speed_value", {
    content = "3.0",
    font_size = 20
})
Set_ui_style("enemy_speed_value", styles.label)
Set_ui_transform("enemy_speed_value", {
    x = 56, y = 40, z = 0,
    w = 6, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("enemy_speed_up_button")
Set_ui_text("enemy_speed_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("enemy_speed_up_button", styles.main_menu)
Set_ui_transform("enemy_speed_up_button", {
    x = 63, y = 40, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Player Health
Create_ui_text("player_health_label", {
    content = "Player Health:",
    font_size = 20
})
Set_ui_style("player_health_label", styles.label)
Set_ui_transform("player_health_label", {
    x = 20, y = 48, z = 0,
    w = 25, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("player_health_down_button")
Set_ui_text("player_health_down_button", {
    content = "-",
    font_size = 24
})
Set_ui_style("player_health_down_button", styles.main_menu)
Set_ui_transform("player_health_down_button", {
    x = 50, y = 48, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_text("player_health_value", {
    content = "100",
    font_size = 20
})
Set_ui_style("player_health_value", styles.label)
Set_ui_transform("player_health_value", {
    x = 56, y = 48, z = 0,
    w = 6, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("player_health_up_button")
Set_ui_text("player_health_up_button", {
    content = "+",
    font_size = 24
})
Set_ui_style("player_health_up_button", styles.main_menu)
Set_ui_transform("player_health_up_button", {
    x = 63, y = 48, z = 0,
    w = 5, h = 5,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Presets
Create_ui_button("preset_easy_button")
Set_ui_text("preset_easy_button", {
    content = "Easy",
    font_size = 28
})
Set_ui_style("preset_easy_button", styles.main_menu)
Set_ui_transform("preset_easy_button", {
    x = 20, y = 63, z = 0,
    w = 15, h = 6,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("preset_normal_button")
Set_ui_text("preset_normal_button", {
    content = "Normal",
    font_size = 28
})
Set_ui_style("preset_normal_button", styles.main_menu)
Set_ui_transform("preset_normal_button", {
    x = 38, y = 63, z = 0,
    w = 15, h = 6,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

Create_ui_button("preset_hard_button")
Set_ui_text("preset_hard_button", {
    content = "Hard",
    font_size = 28
})
Set_ui_style("preset_hard_button", styles.main_menu)
Set_ui_transform("preset_hard_button", {
    x = 56, y = 63, z = 0,
    w = 15, h = 6,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Preset Description
Create_ui_text("preset_description", {
    content = "Presets adjust: Player Health, Enemy count, Health, Speed, Bullet limit, and more",
    font_size = 18
})
Set_ui_style("preset_description", styles.label)
Set_ui_transform("preset_description", {
    x = 0, y = 73, z = 0,
    w = 100, h = 4,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Back Button
Create_ui_button("back_button")
Set_ui_text("back_button", {
    content = "Back",
    font_size = 20
})
Set_ui_style("back_button", styles.main_menu)
Set_ui_transform("back_button", {
    x = 2.31, y = 90.28, z = 0,
    w = 9.26, h = 6.94,
    anchor_x = 0, anchor_y = 0,
    rotation = 0
})

-- Navigation setup
Set_ui_navigation("max_shooter_down_button", { up = "back_button", down = "enemy_health_down_button", right = "max_shooter_up_button" })
Set_ui_navigation("max_shooter_up_button", { up = "back_button", down = "enemy_health_up_button", left = "max_shooter_down_button" })
Set_ui_navigation("enemy_health_down_button", { up = "max_shooter_down_button", down = "enemy_speed_down_button", right = "enemy_health_up_button" })
Set_ui_navigation("enemy_health_up_button", { up = "max_shooter_up_button", down = "enemy_speed_up_button", left = "enemy_health_down_button" })
Set_ui_navigation("enemy_speed_down_button", { up = "enemy_health_down_button", down = "player_health_down_button", right = "enemy_speed_up_button" })
Set_ui_navigation("enemy_speed_up_button", { up = "enemy_health_up_button", down = "player_health_up_button", left = "enemy_speed_down_button" })
Set_ui_navigation("player_health_down_button", { up = "enemy_speed_down_button", down = "preset_easy_button", right = "player_health_up_button" })
Set_ui_navigation("player_health_up_button", { up = "enemy_speed_up_button", down = "preset_normal_button", left = "player_health_down_button" })
Set_ui_navigation("preset_easy_button", { up = "player_health_down_button", down = "back_button", right = "preset_normal_button" })
Set_ui_navigation("preset_normal_button", { up = "player_health_up_button", down = "back_button", left = "preset_easy_button", right = "preset_hard_button" })
Set_ui_navigation("preset_hard_button", { up = "player_health_up_button", down = "back_button", left = "preset_normal_button" })
Set_ui_navigation("back_button", { up = "preset_easy_button", down = "max_shooter_down_button" })
