#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "raylib.h"
#include "systems/client_systems.h"

#include <string>

using namespace engn;

namespace {
constexpr int k_volume_min = 0;
constexpr int k_volume_max = 100;
constexpr int k_volume_step = 5;
} // namespace

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt);
static void update_volume_text(EngineContext& ctx);
static void set_text_if_exists(EngineContext& ctx, const char* tag, const std::string& value);
static void adjust_volume(EngineContext& ctx, int delta);
static void apply_audio_levels(EngineContext& ctx);
static void set_music_volume(EngineContext& ctx, int delta);
static void set_sfx_volume(EngineContext& ctx, int delta);
static void toggle_master_mute(EngineContext& ctx);
static void toggle_music_mute(EngineContext& ctx);
static void toggle_sfx_mute(EngineContext& ctx);

void handle_volume_menu_ui_events(engn::EngineContext& engine_ctx) {
    const auto& evts = engine_ctx.ui_event_queue;

    bool switched_scene = false;
    evts.for_each<evts::UIButtonClicked>([&engine_ctx, &switched_scene](const evts::UIButtonClicked& evt) {
        if (handle_ui_button_clicked(engine_ctx, evt))
            switched_scene = true;
    });
    if (switched_scene)
        return;

    const evts::KeyPressed* key_evt = engine_ctx.input_event_queue.get_last<evts::KeyPressed>();
    const evts::ControllerButtonPressed* pad_evt =
        engine_ctx.input_event_queue.get_last<evts::ControllerButtonPressed>();
    const bool k_escape_pressed = key_evt && key_evt->keycode == evts::KeyboardKeyCode::KeyEscape;
    const bool k_pause_pressed =
        pad_evt && (pad_evt->button == evts::ControllerButton::ControllerButtonStart ||
                    pad_evt->button == evts::ControllerButton::ControllerButtonBack);
    if (k_escape_pressed || k_pause_pressed) {
        if (engine_ctx.settings_return_scene != "") {
            engine_ctx.set_scene(engine_ctx.settings_return_scene);
            return;
        }
    }

    update_volume_text(engine_ctx);
}

static bool handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(evt.tag);

    if (tag_name == "back_button") {
        ctx.set_scene(ctx.settings_return_scene);
        return true;
    } else if (tag_name == "exit_button") {
        ctx.should_quit = true;
        return true;
    } else if (tag_name == "main_menu_button") {
        ctx.settings_return_scene = 1;
        ctx.set_scene("main_menu");
        return true;
    } else if (tag_name == "nav_controls_button") {
        ctx.set_scene("keyboard_settings");
        return true;
    } else if (tag_name == "nav_gamepad_button") {
        ctx.set_scene("gamepad_settings");
        return true;
    } else if (tag_name == "nav_audio_button") {
        ctx.set_scene("audio_settings");
        return true;
    } else if (tag_name == "nav_graphics_button") {
        ctx.set_scene("graphics_settings");
        return true;
    } else if (tag_name == "general_volume_down_button") {
        adjust_volume(ctx, -k_volume_step);
    } else if (tag_name == "general_volume_up_button") {
        adjust_volume(ctx, k_volume_step);
    } else if (tag_name == "music_volume_down_button") {
        set_music_volume(ctx, -k_volume_step);
    } else if (tag_name == "music_volume_up_button") {
        set_music_volume(ctx, k_volume_step);
    } else if (tag_name == "sfx_volume_down_button") {
        set_sfx_volume(ctx, -k_volume_step);
    } else if (tag_name == "sfx_volume_up_button") {
        set_sfx_volume(ctx, k_volume_step);
    } else if (tag_name == "general_mute_button") {
        toggle_master_mute(ctx);
    } else if (tag_name == "music_mute_button") {
        toggle_music_mute(ctx);
    } else if (tag_name == "sfx_mute_button") {
        toggle_sfx_mute(ctx);
    }
    return false;
}

static void set_text_if_exists(EngineContext& ctx, const char* tag, const std::string& value) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value())
        return;
    auto& texts = ctx.registry.get_components<cpnt::UIText>();
    auto& text = texts[ent_opt.value()];
    if (!text.has_value())
        return;
    text->content = value;
}

static void update_volume_text(EngineContext& ctx) {
    if (ctx.master_volume < k_volume_min)
        ctx.master_volume = k_volume_min;
    if (ctx.master_volume > k_volume_max)
        ctx.master_volume = k_volume_max;
    if (ctx.last_master_volume < k_volume_min)
        ctx.last_master_volume = k_volume_min;
    if (ctx.last_master_volume > k_volume_max)
        ctx.last_master_volume = k_volume_max;
    if (ctx.music_volume < k_volume_min)
        ctx.music_volume = k_volume_min;
    if (ctx.music_volume > k_volume_max)
        ctx.music_volume = k_volume_max;
    if (ctx.last_music_volume < k_volume_min)
        ctx.last_music_volume = k_volume_min;
    if (ctx.last_music_volume > k_volume_max)
        ctx.last_music_volume = k_volume_max;
    if (ctx.sfx_volume < k_volume_min)
        ctx.sfx_volume = k_volume_min;
    if (ctx.sfx_volume > k_volume_max)
        ctx.sfx_volume = k_volume_max;
    if (ctx.last_sfx_volume < k_volume_min)
        ctx.last_sfx_volume = k_volume_min;
    if (ctx.last_sfx_volume > k_volume_max)
        ctx.last_sfx_volume = k_volume_max;

    set_text_if_exists(ctx, "general_volume_value", std::to_string(ctx.master_volume) + "%");
    set_text_if_exists(ctx, "music_volume_value", std::to_string(ctx.music_volume) + "%");
    set_text_if_exists(ctx, "sfx_volume_value", std::to_string(ctx.sfx_volume) + "%");
    set_text_if_exists(ctx, "general_mute_button", ctx.master_muted ? "Unmute" : "Mute");
    set_text_if_exists(ctx, "music_mute_button", ctx.music_muted ? "Unmute" : "Mute");
    set_text_if_exists(ctx, "sfx_mute_button", ctx.sfx_muted ? "Unmute" : "Mute");
    apply_audio_levels(ctx);
}

static void adjust_volume(EngineContext& ctx, int delta) {
    if (ctx.master_muted) {
        ctx.last_master_volume += delta;
        if (ctx.last_master_volume < k_volume_min)
            ctx.last_master_volume = k_volume_min;
        if (ctx.last_master_volume > k_volume_max)
            ctx.last_master_volume = k_volume_max;
    } else {
        ctx.master_volume += delta;
        if (ctx.master_volume < k_volume_min)
            ctx.master_volume = k_volume_min;
        if (ctx.master_volume > k_volume_max)
            ctx.master_volume = k_volume_max;
    }
    update_volume_text(ctx);
}

static void set_music_volume(EngineContext& ctx, int delta) {
    if (ctx.music_muted) {
        ctx.last_music_volume += delta;
        if (ctx.last_music_volume < k_volume_min)
            ctx.last_music_volume = k_volume_min;
        if (ctx.last_music_volume > k_volume_max)
            ctx.last_music_volume = k_volume_max;
    } else {
        ctx.music_volume += delta;
        if (ctx.music_volume < k_volume_min)
            ctx.music_volume = k_volume_min;
        if (ctx.music_volume > k_volume_max)
            ctx.music_volume = k_volume_max;
    }
    update_volume_text(ctx);
}

static void set_sfx_volume(EngineContext& ctx, int delta) {
    if (ctx.sfx_muted) {
        ctx.last_sfx_volume += delta;
        if (ctx.last_sfx_volume < k_volume_min)
            ctx.last_sfx_volume = k_volume_min;
        if (ctx.last_sfx_volume > k_volume_max)
            ctx.last_sfx_volume = k_volume_max;
    } else {
        ctx.sfx_volume += delta;
        if (ctx.sfx_volume < k_volume_min)
            ctx.sfx_volume = k_volume_min;
        if (ctx.sfx_volume > k_volume_max)
            ctx.sfx_volume = k_volume_max;
    }
    update_volume_text(ctx);
}

static void apply_audio_levels(EngineContext& ctx) {
    const float k_master = static_cast<float>(ctx.master_volume) / static_cast<float>(k_volume_max);
    const float k_music = k_master * static_cast<float>(ctx.music_volume) / static_cast<float>(k_volume_max);
    const float k_sfx = k_master * static_cast<float>(ctx.sfx_volume) / static_cast<float>(k_volume_max);

    SetMasterVolume(k_master);

    auto music_asset = ctx.assets_manager.get_asset<Music>("battle_music");
    if (music_asset.has_value()) {
        SetMusicVolume(music_asset.value(), k_music);
    }

    auto sound_asset = ctx.assets_manager.get_asset<Sound>("shoot_sound");
    if (sound_asset.has_value()) {
        SetSoundVolume(sound_asset.value(), k_sfx);
    }
}

static void toggle_master_mute(EngineContext& ctx) {
    if (!ctx.master_muted) {
        ctx.last_master_volume = ctx.master_volume;
        ctx.master_volume = k_volume_min;
        ctx.master_muted = true;
    } else {
        ctx.master_volume = ctx.last_master_volume;
        ctx.master_muted = false;
    }
    update_volume_text(ctx);
}

static void toggle_music_mute(EngineContext& ctx) {
    if (!ctx.music_muted) {
        ctx.last_music_volume = ctx.music_volume;
        ctx.music_volume = k_volume_min;
        ctx.music_muted = true;
    } else {
        ctx.music_volume = ctx.last_music_volume;
        ctx.music_muted = false;
    }
    update_volume_text(ctx);
}

static void toggle_sfx_mute(EngineContext& ctx) {
    if (!ctx.sfx_muted) {
        ctx.last_sfx_volume = ctx.sfx_volume;
        ctx.sfx_volume = k_volume_min;
        ctx.sfx_muted = true;
    } else {
        ctx.sfx_volume = ctx.last_sfx_volume;
        ctx.sfx_muted = false;
    }
    update_volume_text(ctx);
}
