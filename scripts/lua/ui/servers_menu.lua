-- Server browser UI
local styles = {
    main_menu = {
        background_color =          {r = 50,  g = 50,  b = 50,  a = 255},
        background_color_hovered =  {r = 70,  g = 70,  b = 70,  a = 255},
        background_color_pressed =  {r = 30,  g = 30,  b = 30,  a = 255},
        text_color =                {r = 220, g = 220, b = 220, a = 255},
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
        text_color =                {r = 220, g = 220, b = 220, a = 255},
        text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
        text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
        border_color =              {r = 0, g = 0, b = 0, a = 0},
        border_color_hovered =      {r = 0, g = 0, b = 0, a = 0},
        border_color_pressed =      {r = 0, g = 0, b = 0, a = 0},
        border_radius = 0,
        border_thickness = 0
    }
}

Create_ui_text("servers_title", {
    content = "Multiplayer - Servers",
    font_size = 70
})

Set_ui_style("servers_title", styles.title)

Set_ui_transform("servers_title", {
    x = 0,
    y = 3,
    z = 0,
    w = 100,
    h = 12,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Server list background panel
Create_ui_text("server_list_bg", {
    content = "",
    font_size = 1
})

Set_ui_style("server_list_bg", {
    background_color =          {r = 25,  g = 25,  b = 25,  a = 255},
    background_color_hovered =  {r = 25,  g = 25,  b = 25,  a = 255},
    background_color_pressed =  {r = 25,  g = 25,  b = 25,  a = 255},
    text_color =                {r = 0, g = 0, b = 0, a = 0},
    text_color_hovered =        {r = 0, g = 0, b = 0, a = 0},
    text_color_pressed =        {r = 0, g = 0, b = 0, a = 0},
    border_color =              {r = 80, g = 80, b = 80, a = 255},
    border_color_hovered =      {r = 80, g = 80, b = 80, a = 255},
    border_color_pressed =      {r = 80, g = 80, b = 80, a = 255},
    border_radius = 0.3,
    border_thickness = 3
})

Set_ui_transform("server_list_bg", {
    x = 8,
    y = 16,
    z = 0,
    w = 84,
    h = 38,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Input labels and fields
Create_ui_text("server_name_label", {
    content = "Name:",
    font_size = 22
})
Set_ui_style("server_name_label", styles.title)
Set_ui_transform("server_name_label", {
    x = 18,
    y = 62,
    z = 0,
    w = 30,
    h = 4,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field("server_name_input", "My server")
Set_ui_style("server_name_input", styles.main_menu)
Set_ui_text("server_name_input", {
    content = "My server",
    font_size = 22
})
Set_ui_transform("server_name_input", {
    x = 18,
    y = 66.5,
    z = 0,
    w = 30,
    h = 5,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("server_ip_label", {
    content = "IP:",
    font_size = 22
})
Set_ui_style("server_ip_label", styles.title)
Set_ui_transform("server_ip_label", {
    x = 56,
    y = 62,
    z = 0,
    w = 20,
    h = 4,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field("server_ip_input", "127.0.0.1")
Set_ui_style("server_ip_input", styles.main_menu)
Set_ui_text("server_ip_input", {
    content = "127.0.0.1",
    font_size = 22
})
Set_ui_transform("server_ip_input", {
    x = 56,
    y = 66.5,
    z = 0,
    w = 20,
    h = 5,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_text("server_port_label", {
    content = "Port:",
    font_size = 22
})
Set_ui_style("server_port_label", styles.title)
Set_ui_transform("server_port_label", {
    x = 84,
    y = 62,
    z = 0,
    w = 8,
    h = 4,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field("server_port_input", "8080")
Set_ui_style("server_port_input", styles.main_menu)
Set_ui_text("server_port_input", {
    content = "8080",
    font_size = 22
})
Set_ui_transform("server_port_input", {
    x = 84,
    y = 66.5,
    z = 0,
    w = 8,
    h = 5,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Buttons
Create_ui_button("add_server_button")
Set_ui_text("add_server_button", {
    content = "Add",
    font_size = 26
})
Set_ui_style("add_server_button", styles.main_menu)
Set_ui_transform("add_server_button", {
    x = 11,
    y = 78,
    z = 0,
    w = 18,
    h = 6,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("delete_server_button")
Set_ui_text("delete_server_button", {
    content = "Delete",
    font_size = 26
})
Set_ui_style("delete_server_button", styles.main_menu)
Set_ui_transform("delete_server_button", {
    x = 31,
    y = 78,
    z = 0,
    w = 18,
    h = 6,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("join_server_button")
Set_ui_text("join_server_button", {
    content = "Join",
    font_size = 26
})
Set_ui_style("join_server_button", styles.main_menu)
Set_ui_transform("join_server_button", {
    x = 51,
    y = 78,
    z = 0,
    w = 18,
    h = 6,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_button("back_button")
Set_ui_text("back_button", {
    content = "Back",
    font_size = 26
})
Set_ui_style("back_button", styles.main_menu)
Set_ui_transform("back_button", {
    x = 71,
    y = 78,
    z = 0,
    w = 18,
    h = 6,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Status text
Create_ui_text("status_text", {
    content = "Select a server or add a new one",
    font_size = 18
})
Set_ui_style("status_text", styles.title)
Set_ui_transform("status_text", {
    x = 8,
    y = 88,
    z = 0,
    w = 84,
    h = 4,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation("server_name_input", { right = "server_ip_input", down = "add_server_button" })
Set_ui_navigation("server_ip_input", { left = "server_name_input", down = "join_server_button", right = "server_port_input" })
Set_ui_navigation("server_port_input", { left = "server_ip_input", down = "back_button" })
Set_ui_navigation("add_server_button", { up = "server_name_input", right = "delete_server_button" })
Set_ui_navigation("delete_server_button", { up = "server_name_input", left = "add_server_button", right = "join_server_button" })
Set_ui_navigation("join_server_button", { up = "server_ip_input", left = "delete_server_button", right = "back_button" })
Set_ui_navigation("back_button", { up = "server_port_input", left = "join_server_button" })
