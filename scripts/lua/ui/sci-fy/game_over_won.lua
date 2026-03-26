local function include_shared(script_name)
    local scify_path = "scripts/lua/ui/sci-fy/" .. script_name
    local ui_root_path = "scripts/lua/ui/" .. script_name
    local loaded = pcall(dofile, scify_path)
    if not loaded then
        dofile(ui_root_path)
    end
end

include_shared("_theme.lua")
local styles = UITheme.presets.game_over_won

Create_ui_text("backdrop", { content = "", font_size = 1 })
Set_ui_style("backdrop", styles.backdrop)
Set_ui_transform("backdrop", { x = 0, y = 0, z = 0, w = 100, h = 100, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("frame", { content = "", font_size = 1 })
Set_ui_style("frame", styles.frame)
Set_ui_transform("frame", { x = 18, y = 10, z = 1, w = 64, h = 80, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("frame_accent_top", { content = "", font_size = 1 })
Set_ui_style("frame_accent_top", styles.accent)
Set_ui_transform("frame_accent_top", { x = 22, y = 14, z = 2, w = 18, h = 1.2, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("frame_accent_bottom", { content = "", font_size = 1 })
Set_ui_style("frame_accent_bottom", styles.accent)
Set_ui_transform("frame_accent_bottom", { x = 60, y = 86, z = 2, w = 18, h = 1.2, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("game_over_title", { content = "MISSION CLEAR", font_size = 72 })
Set_ui_style("game_over_title", styles.title)
Set_ui_transform("game_over_title", { x = 24, y = 16, z = 3, w = 52, h = 12, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("game_over_subtitle", { content = "Boss target eliminated. Sector secured.", font_size = 22 })
Set_ui_style("game_over_subtitle", styles.subtitle)
Set_ui_transform("game_over_subtitle", { x = 24, y = 28, z = 3, w = 52, h = 5, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("stats_panel", { content = "", font_size = 1 })
Set_ui_style("stats_panel", styles.stat_panel)
Set_ui_transform("stats_panel", { x = 24, y = 35, z = 2, w = 52, h = 32, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("score_label", { content = "TOTAL SCORE", font_size = 18 })
Set_ui_style("score_label", styles.stat_label)
Set_ui_transform("score_label", { x = 28, y = 38, z = 3, w = 28, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("score_value", { content = "000000", font_size = 36 })
Set_ui_style("score_value", styles.stat_value)
Set_ui_transform("score_value", { x = 28, y = 41, z = 3, w = 44, h = 6, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("kills_label", { content = "KILLS", font_size = 18 })
Set_ui_style("kills_label", styles.stat_label)
Set_ui_transform("kills_label", { x = 28, y = 51.5, z = 3, w = 20, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("kills_value", { content = "0", font_size = 24 })
Set_ui_style("kills_value", styles.stat_value)
Set_ui_transform("kills_value", { x = 54, y = 51.5, z = 3, w = 18, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("level_label", { content = "LEVEL", font_size = 18 })
Set_ui_style("level_label", styles.stat_label)
Set_ui_transform("level_label", { x = 28, y = 55.5, z = 3, w = 20, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("level_value", { content = "1", font_size = 24 })
Set_ui_style("level_value", styles.stat_value)
Set_ui_transform("level_value", { x = 54, y = 55.5, z = 3, w = 18, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("boss_killed_label", { content = "BOSS KILLS", font_size = 18 })
Set_ui_style("boss_killed_label", styles.stat_label)
Set_ui_transform("boss_killed_label", { x = 28, y = 59.5, z = 3, w = 20, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("boss_killed_value", { content = "0 / 0", font_size = 24 })
Set_ui_style("boss_killed_value", styles.stat_value)
Set_ui_transform("boss_killed_value", { x = 54, y = 59.5, z = 3, w = 20, h = 3, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_text("continue_hint", { content = "Choose your next move", font_size = 20 })
Set_ui_style("continue_hint", styles.subtitle)
Set_ui_transform("continue_hint", { x = 24, y = 68.5, z = 3, w = 40, h = 4, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_button("start_guard_button")
Set_ui_text("start_guard_button", { content = "", font_size = 1 })
Set_ui_style("start_guard_button", styles.dummy_button)
Set_ui_transform("start_guard_button", { x = 0, y = 0, z = 0, w = 0, h = 0, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_button("retry_button")
Set_ui_text("retry_button", { content = "Retry", font_size = 26 })
Set_ui_style("retry_button", styles.primary_button)
Set_ui_transform("retry_button", { x = 29, y = 74, z = 3, w = 18, h = 6.2, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_button("main_menu_button")
Set_ui_text("main_menu_button", { content = "Main Menu", font_size = 26 })
Set_ui_style("main_menu_button", styles.secondary_button)
Set_ui_transform("main_menu_button", { x = 51, y = 74, z = 3, w = 22, h = 6.2, anchor_x = 0, anchor_y = 0, rotation = 0 })

Create_ui_button("exit_button")
Set_ui_text("exit_button", { content = "Exit", font_size = 22 })
Set_ui_style("exit_button", styles.secondary_button)
Set_ui_transform("exit_button", { x = 30, y = 83, z = 3, w = 42, h = 5.2, anchor_x = 0, anchor_y = 0, rotation = 0 })

Set_ui_navigation("start_guard_button", { up = "retry_button", down = "retry_button", left = "retry_button", right = "retry_button" })
Set_ui_navigation("retry_button", { up = "exit_button", right = "main_menu_button", down = "exit_button" })
Set_ui_navigation("main_menu_button", { left = "retry_button", up = "exit_button", down = "exit_button" })
Set_ui_navigation("exit_button", { up = "main_menu_button", down = "retry_button" })
