-- Lobby browser UI
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
    lobby_item = {
        background_color =          {r = 45,  g = 45,  b = 45,  a = 255},
        background_color_hovered =  {r = 80,  g = 80,  b = 80,  a = 255},
        background_color_pressed =  {r = 35,  g = 35,  b = 35,  a = 255},
        text_color =                {r = 220, g = 220, b = 220, a = 255},
        text_color_hovered =        {r = 255, g = 255, b = 255, a = 255},
        text_color_pressed =        {r = 180, g = 180, b = 180, a = 255},
        border_color =              {r = 100, g = 100, b = 100, a = 255},
        border_color_hovered =      {r = 150, g = 150, b = 150, a = 255},
        border_color_pressed =      {r = 200, g = 200, b = 200, a = 255},
        border_radius = 0.3,
        border_thickness = 3
    }
}

-- Title
Create_ui_text(2, "titlescreen", {
    content = "Multiplayer Lobbies",
    font_size = 80
})

Set_ui_style(2, "titlescreen", styles.title)

Set_ui_transform(2, "titlescreen", {
    x = 100,
    y = 20,
    z = 0,
    w = 880,
    h = 100,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Server connection input (IP)
Create_ui_text(2, "server_label", {
    content = "Server IP:",
    font_size = 24
})

Set_ui_style(2, "server_label", styles.title)

Set_ui_transform(2, "server_label", {
    x = 50,
    y = 130,
    z = 0,
    w = 150,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field(2, "lobby_server_ip", "127.0.0.1")

Set_ui_style(2, "lobby_server_ip", styles.main_menu)

Set_ui_text(2, "lobby_server_ip", {
    content = "127.0.0.1",
    font_size = 24
})

Set_ui_transform(2, "lobby_server_ip", {
    x = 200,
    y = 130,
    z = 0,
    w = 250,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Port input
Create_ui_text(2, "port_label", {
    content = "Port:",
    font_size = 24
})

Set_ui_style(2, "port_label", styles.title)

Set_ui_transform(2, "port_label", {
    x = 470,
    y = 130,
    z = 0,
    w = 80,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Create_ui_input_field(2, "lobby_server_port", "8080")

Set_ui_style(2, "lobby_server_port", styles.main_menu)

Set_ui_text(2, "lobby_server_port", {
    content = "8080",
    font_size = 24
})

Set_ui_transform(2, "lobby_server_port", {
    x = 550,
    y = 130,
    z = 0,
    w = 150,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Connect button
Create_ui_button(2, "connect_to_server_button")

Set_ui_text(2, "connect_to_server_button", {
    content = "Connect",
    font_size = 24
})

Set_ui_style(2, "connect_to_server_button", styles.main_menu)

Set_ui_transform(2, "connect_to_server_button", {
    x = 720,
    y = 130,
    z = 0,
    w = 150,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Refresh lobbies button
Create_ui_button(2, "refresh_lobbies_button")

Set_ui_text(2, "refresh_lobbies_button", {
    content = "Refresh",
    font_size = 28
})

Set_ui_style(2, "refresh_lobbies_button", styles.main_menu)

Set_ui_transform(2, "refresh_lobbies_button", {
    x = 900,
    y = 130,
    z = 0,
    w = 150,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Create lobby section
Create_ui_text(2, "create_lobby_label", {
    content = "Create New Lobby:",
    font_size = 28
})

Set_ui_style(2, "create_lobby_label", styles.title)

Set_ui_transform(2, "create_lobby_label", {
    x = 50,
    y = 190,
    z = 0,
    w = 300,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Lobby name input
Create_ui_input_field(2, "lobby_name_input", "My Lobby")

Set_ui_style(2, "lobby_name_input", styles.main_menu)

Set_ui_text(2, "lobby_name_input", {
    content = "My Lobby",
    font_size = 24
})

Set_ui_transform(2, "lobby_name_input", {
    x = 50,
    y = 240,
    z = 0,
    w = 300,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Create lobby button
Create_ui_button(2, "create_lobby_button")

Set_ui_text(2, "create_lobby_button", {
    content = "Create Lobby",
    font_size = 28
})

Set_ui_style(2, "create_lobby_button", styles.main_menu)

Set_ui_transform(2, "create_lobby_button", {
    x = 370,
    y = 240,
    z = 0,
    w = 200,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Available lobbies section
Create_ui_text(2, "available_lobbies_label", {
    content = "Available Lobbies:",
    font_size = 28
})

Set_ui_style(2, "available_lobbies_label", styles.title)

Set_ui_transform(2, "available_lobbies_label", {
    x = 50,
    y = 300,
    z = 0,
    w = 300,
    h = 40,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Lobby list background panel
Create_ui_text(2, "lobby_list_bg", {
    content = "",
    font_size = 1
})

Set_ui_style(2, "lobby_list_bg", {
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

Set_ui_transform(2, "lobby_list_bg", {
    x = 50,
    y = 350,
    z = 0,
    w = 980,
    h = 280,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Status text (for showing connection/error messages)
Create_ui_text(2, "status_text", {
    content = "Connect to a server to list lobbies",
    font_size = 20
})

Set_ui_style(2, "status_text", styles.title)

Set_ui_transform(2, "status_text", {
    x = 50,
    y = 640,
    z = 0,
    w = 700,
    h = 30,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Back button
Create_ui_button(2, "back_button")

Set_ui_text(2, "back_button", {
    content = "Back",
    font_size = 28
})

Set_ui_style(2, "back_button", styles.main_menu)

Set_ui_transform(2, "back_button", {
    x = 850,
    y = 650,
    z = 0,
    w = 180,
    h = 50,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation(2, "connect_to_server_button", { up = "back_button", down = "refresh_lobbies_button" })
Set_ui_navigation(2, "refresh_lobbies_button", { up = "connect_to_server_button", down = "create_lobby_button" })
Set_ui_navigation(2, "create_lobby_button", { up = "refresh_lobbies_button", down = "back_button" })
Set_ui_navigation(2, "back_button", { up = "create_lobby_button", down = "connect_to_server_button" })
