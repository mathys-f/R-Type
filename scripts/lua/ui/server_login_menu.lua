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

Create_ui_text(2, "titlescreen", {
    content = "Connect to a server",
    font_size = 80
})

Set_ui_style(2, "titlescreen", styles.title)

Set_ui_transform(2, "titlescreen", {
    x = 30,
    y = 0,
    z = 0,
    w = 750,
    h = 150,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(2, "connect_button")

Set_ui_text(2, "connect_button", {
    content = "Connect",
    font_size = 32
})

Set_ui_style(2, "connect_button", styles.main_menu)

Set_ui_transform(2, "connect_button", {
    x = 375,
    y = 550,
    z = 0,
    w = 250,
    h = 75,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field(2, "ip_input_field", "127.0.0.1")

Set_ui_style(2, "ip_input_field", styles.main_menu)

Set_ui_text(2, "ip_input_field", {
    content = "127.0.0.1",
    font_size = 48
})

Set_ui_transform(2, "ip_input_field", {
    x = 300,
    y = 300,
    z = 0,
    w = 400,
    h = 75,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field(2, "port_input_field", "8766")

Set_ui_style(2, "port_input_field", styles.main_menu)

Set_ui_text(2, "port_input_field", {
    content = "8080",
    font_size = 48
})

Set_ui_transform(2, "port_input_field", {
    x = 300,
    y = 400,
    z = 0,
    w = 400,
    h = 75,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button(2, "back_button")

Set_ui_style(2, "back_button", styles.main_menu)

Set_ui_text(2, "back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform(2, "back_button", {
    x = 25,
    y = 650,
    z = 0,
    w = 100,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
