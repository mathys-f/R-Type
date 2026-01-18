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
Create_ui_text("titlescreen", {
    content = "Multiplayer Lobbies",
    font_size = 80
})

Set_ui_style("titlescreen", styles.title)

Set_ui_transform("titlescreen", {
    x = 9.26,
    y = 2.78,
    z = 0,
    w = 81.48,
    h = 13.89,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Refresh lobbies button
Create_ui_button("refresh_lobbies_button")

Set_ui_text("refresh_lobbies_button", {
    content = "Refresh",
    font_size = 28
})

Set_ui_style("refresh_lobbies_button", styles.main_menu)

Set_ui_transform("refresh_lobbies_button", {
    x = 74.07,
    y = 18.06,
    z = 0,
    w = 21.3,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Create lobby section
Create_ui_text("create_lobby_label", {
    content = "Create New Lobby:",
    font_size = 28
})

Set_ui_style("create_lobby_label", styles.title)

Set_ui_transform("create_lobby_label", {
    x = 4.63,
    y = 20.83,
    z = 0,
    w = 27.78,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Lobby name input
Create_ui_input_field("lobby_name_input", "My Lobby")

Set_ui_style("lobby_name_input", styles.main_menu)

Set_ui_text("lobby_name_input", {
    content = "My Lobby",
    font_size = 24
})

Set_ui_transform("lobby_name_input", {
    x = 4.63,
    y = 27.78,
    z = 0,
    w = 27.78,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Create lobby button
Create_ui_button("create_lobby_button")

Set_ui_text("create_lobby_button", {
    content = "Create Lobby",
    font_size = 28
})

Set_ui_style("create_lobby_button", styles.main_menu)

Set_ui_transform("create_lobby_button", {
    x = 34.26,
    y = 27.78,
    z = 0,
    w = 18.52,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Available lobbies section
Create_ui_text("available_lobbies_label", {
    content = "Available Lobbies:",
    font_size = 28
})

Set_ui_style("available_lobbies_label", styles.title)

Set_ui_transform("available_lobbies_label", {
    x = 4.63,
    y = 36.11,
    z = 0,
    w = 27.78,
    h = 5.56,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Lobby list background panel
Create_ui_text("lobby_list_bg", {
    content = "",
    font_size = 1
})

Set_ui_style("lobby_list_bg", {
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

Set_ui_transform("lobby_list_bg", {
    x = 4.63,
    y = 43.06,
    z = 0,
    w = 90.74,
    h = 42.36,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Status text (for showing connection/error messages)
Create_ui_text("status_text", {
    content = "Connect to a server to list lobbies",
    font_size = 20
})

Set_ui_style("status_text", styles.title)

Set_ui_transform("status_text", {
    x = 4.63,
    y = 86.11,
    z = 0,
    w = 64.81,
    h = 4.17,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- Back button
Create_ui_button("back_button")

Set_ui_text("back_button", {
    content = "Back",
    font_size = 28
})

Set_ui_style("back_button", styles.main_menu)

Set_ui_transform("back_button", {
    x = 78.7,
    y = 88.89,
    z = 0,
    w = 16.67,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

Set_ui_navigation("refresh_lobbies_button", { down = "create_lobby_button" })
Set_ui_navigation("lobby_name_input", { up = "refresh_lobbies_button", right = "create_lobby_button", down = "back_button" })
Set_ui_navigation("create_lobby_button", { up = "refresh_lobbies_button", left = "lobby_name_input", down = "back_button" })
Set_ui_navigation("back_button", { up = "create_lobby_button", down = "refresh_lobbies_button" })
