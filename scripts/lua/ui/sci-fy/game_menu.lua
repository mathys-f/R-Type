local function include_shared(script_name)
    local scify_path = "scripts/lua/ui/sci-fy/" .. script_name
    local ui_root_path = "scripts/lua/ui/" .. script_name
    local loaded = pcall(dofile, scify_path)
    if not loaded then
        dofile(ui_root_path)
    end
end

include_shared("_theme.lua")
local styles = UITheme.presets.game_menu

Create_ui_button("back_button")

Set_ui_style("back_button", styles.main_menu)

Set_ui_text("back_button", {
    content = "Back",
    font_size = 20
})

Set_ui_transform("back_button", {
    x = 2.31,
    y = 90.28,
    z = 0,
    w = 9.26,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})
