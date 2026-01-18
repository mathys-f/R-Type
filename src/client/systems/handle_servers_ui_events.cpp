#include "game_engine/components/tag.h"
#include "game_engine/components/ui/ui_clip.h"
#include "game_engine/components/ui/ui_focusable.h"
#include "game_engine/components/ui/ui_navigation.h"
#include "game_engine/components/ui/ui_scroll_area.h"
#include "game_engine/components/ui/ui_transform.h"
#include "game_engine/engine.h"
#include "game_engine/network_client.h"
#include "systems/client_systems.h"
#include "utils/color.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace engn;

namespace {
constexpr float k_item_height_px = 50.0f;
constexpr float k_item_spacing_px = 8.0f;
constexpr float k_list_padding_px = 10.0f;
constexpr float k_scroll_epsilon_px = 0.01f;
constexpr float k_scrollbar_width_px = 8.0f;
constexpr float k_scrollbar_padding_px = 2.0f;
constexpr float k_list_right_margin_px = 16.0f;
constexpr float k_scrollbar_min_thumb_px = 14.0f;
constexpr float k_scrollbar_radius = 0.2f;
constexpr float k_scrollbar_border_thickness = 0.0f;
constexpr float k_connect_timeout_sec = 10.0f;
constexpr unsigned char k_scrollbar_track_color = 25U;
constexpr unsigned char k_scrollbar_thumb_color = 140U;
constexpr unsigned char k_scrollbar_track_alpha = 140U;
constexpr unsigned char k_scrollbar_thumb_alpha = 220U;
constexpr unsigned char k_scrollbar_hidden_alpha = 0U;
constexpr float k_uninitialized_scroll_offset_px = -1.0f;
constexpr std::size_t k_digit_base_10 = 10;
constexpr int k_text_font_size = 24;
constexpr int k_tag_prefix_length = 12; // "server_item_"
constexpr int k_max_port_value = 65535;

constexpr unsigned char k_alpha_opaque = 255U;
constexpr unsigned char k_color_dark_bg = 45U;
constexpr unsigned char k_color_darker_bg = 80U;
constexpr unsigned char k_color_darker_pressed = 35U;
constexpr unsigned char k_color_light_text = 220U;
constexpr unsigned char k_color_white = 255U;
constexpr unsigned char k_color_light_gray = 180U;
constexpr unsigned char k_color_medium_gray = 100U;
constexpr unsigned char k_color_medium_light = 150U;
constexpr unsigned char k_color_medium = 200U;
constexpr float k_border_radius = 0.3f;
constexpr float k_border_thickness = 3.0f;

struct UiRectPx {
    float x;
    float y;
    float w;
    float h;
};

struct ServerEntry {
    std::string name;
    std::string ip;
    std::uint16_t port = 0;
};

struct ServersState {
    std::unordered_map<std::string, ServerEntry> servers;
    std::vector<std::string> server_keys;
    std::vector<ecs::TagRegistry::TagId> server_item_tags;
    std::optional<std::size_t> selected_index;
    float last_scroll_offset_px = k_uninitialized_scroll_offset_px;
    std::string status_message = "Select a server or add a new one";
    std::shared_ptr<engn::NetworkClient> network_client;
    bool connecting = false;
    float connect_elapsed_sec = 0.0f;
    std::string pending_ip;
    std::uint16_t pending_port = 0;
    std::optional<std::string> pending_status;
    std::optional<bool> pending_login_success;
    std::mutex mutex;
};

ServersState& get_servers_state() {
    static ServersState s_state;
    return s_state;
}

ecs::TagRegistry::TagId tag_id_or_invalid(const ecs::TagRegistry& tags, const char* name) {
    auto ent_opt = tags.get_entity(name);
    if (!ent_opt.has_value())
        return ecs::TagRegistry::k_invalid_tag_id;
    return tags.get_tag_id(name);
}

void update_navigation_up_only(EngineContext& ctx, const char* tag, ecs::TagRegistry::TagId up) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value())
        return;
    auto& navigations = ctx.registry.get_components<cpnt::UINavigation>();
    const std::size_t k_idx = ent_opt->value();
    if (k_idx < navigations.size() && navigations[k_idx].has_value()) {
        navigations[k_idx]->up_tag = up;
        return;
    }
    ctx.registry.add_component(ent_opt.value(), cpnt::UINavigation{up, ecs::TagRegistry::k_invalid_tag_id,
                                                                   ecs::TagRegistry::k_invalid_tag_id,
                                                                   ecs::TagRegistry::k_invalid_tag_id});
}

void apply_server_list_navigation(EngineContext& ctx, std::size_t server_count) {
    if (server_count == 0)
        return;

    const auto& tags = ctx.registry.get_tag_registry();
    const auto& tag_cache = get_servers_state().server_item_tags;
    ecs::TagRegistry::TagId first = tag_cache[0];
    ecs::TagRegistry::TagId last = tag_cache[server_count - 1];

    update_navigation_up_only(ctx, "server_name_input", last);
    update_navigation_up_only(ctx, "server_ip_input", last);
    update_navigation_up_only(ctx, "server_port_input", last);

    for (std::size_t i = 0; i < server_count; i++) {
        auto tag_id = tag_cache[i];
        if (tag_id == ecs::TagRegistry::k_invalid_tag_id)
            continue;
        auto ent_opt = tags.get_entity(tag_id);
        if (!ent_opt.has_value())
            continue;
        auto up = (i == 0) ? first : tag_cache[i - 1];
        auto down = (i + 1 < server_count) ? tag_cache[i + 1] : tag_id_or_invalid(tags, "server_name_input");
        ctx.registry.add_component(ent_opt.value(),
                                   cpnt::UINavigation{up, down, ecs::TagRegistry::k_invalid_tag_id,
                                                      ecs::TagRegistry::k_invalid_tag_id});
    }
}

void update_status_text(EngineContext& ctx, const std::string& message) {
    auto status_ent_opt = ctx.registry.get_tag_registry().get_entity("status_text");
    if (status_ent_opt.has_value()) {
        auto& text_components = ctx.registry.get_components<cpnt::UIText>();
        if (auto& text = text_components[status_ent_opt.value()]; text.has_value()) {
            text->content = message;
        }
    }
    get_servers_state().status_message = message;
}

bool set_text_for_tag(EngineContext& ctx, const char* tag, const std::string& value) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value())
        return false;
    auto& text_components = ctx.registry.get_components<cpnt::UIText>();
    if (ent_opt->value() >= text_components.size())
        return false;
    if (auto& text = text_components[ent_opt.value()]; text.has_value()) {
        text->content = value;
        return true;
    }
    return false;
}

std::optional<std::string> get_text_for_tag(EngineContext& ctx, const char* tag) {
    auto ent_opt = ctx.registry.get_tag_registry().get_entity(tag);
    if (!ent_opt.has_value())
        return std::nullopt;
    auto& text_components = ctx.registry.get_components<cpnt::UIText>();
    if (ent_opt->value() >= text_components.size())
        return std::nullopt;
    if (auto& text = text_components[ent_opt.value()]; text.has_value())
        return text->content;
    return std::nullopt;
}

const cpnt::UITransform* get_server_list_transform(EngineContext& ctx) {
    auto list_ent_opt = ctx.registry.get_tag_registry().get_entity("server_list_bg");
    if (!list_ent_opt.has_value())
        return nullptr;
    auto& transforms = ctx.registry.get_components<cpnt::UITransform>();
    const auto& list_transform = transforms[list_ent_opt.value()];
    if (!list_transform.has_value())
        return nullptr;
    return &list_transform.value();
}

std::optional<UiRectPx> get_server_list_rect_px(EngineContext& ctx, const cpnt::UITransform& list_transform) {
    const float k_width = ctx.window_size.x;  // NOLINT(cppcoreguidelines-pro-type-union-access)
    const float k_height = ctx.window_size.y; // NOLINT(cppcoreguidelines-pro-type-union-access)

    UiRectPx rect{
        list_transform.x / 100.0f * k_width,
        list_transform.y / 100.0f * k_height,
        list_transform.w / 100.0f * k_width,
        list_transform.h / 100.0f * k_height
    };
    return rect;
}

std::optional<ecs::Entity> get_server_list_entity(EngineContext& ctx) {
    return ctx.registry.get_tag_registry().get_entity("server_list_bg");
}

cpnt::UIScrollArea& ensure_server_scroll_area(EngineContext& ctx, ecs::Entity list_entity) {
    auto& scroll_areas = ctx.registry.register_component<cpnt::UIScrollArea>();
    const std::size_t k_idx = list_entity.value();
    if (k_idx >= scroll_areas.size() || !scroll_areas[k_idx].has_value()) {
        ctx.registry.add_component(list_entity, cpnt::UIScrollArea{});
    }
    auto& scroll_area_opt = scroll_areas[k_idx];

    scroll_area_opt->padding_px = k_list_padding_px;
    scroll_area_opt->scroll_step_px = k_item_height_px + k_item_spacing_px;
    scroll_area_opt->scrollbar_width_px = k_scrollbar_width_px;
    scroll_area_opt->scrollbar_padding_px = k_scrollbar_padding_px;
    scroll_area_opt->scrollbar_min_thumb_px = k_scrollbar_min_thumb_px;
    scroll_area_opt->scrollbar_radius = k_scrollbar_radius;
    scroll_area_opt->scrollbar_border_thickness = k_scrollbar_border_thickness;
    scroll_area_opt->scrollbar_hidden_alpha = k_scrollbar_hidden_alpha;
    scroll_area_opt->scrollbar_track_color = utils::Color{k_scrollbar_track_color, k_scrollbar_track_color,
                                                          k_scrollbar_track_color, k_scrollbar_track_alpha};
    scroll_area_opt->scrollbar_thumb_color = utils::Color{k_scrollbar_thumb_color, k_scrollbar_thumb_color,
                                                          k_scrollbar_thumb_color, k_scrollbar_thumb_alpha};
    return scroll_area_opt.value();
}

float px_to_percent_x(const EngineContext& ctx, float px) {
    return (px / ctx.window_size.x) * 100.0f; // NOLINT(cppcoreguidelines-pro-type-union-access)
}

float px_to_percent_y(const EngineContext& ctx, float px) {
    return (px / ctx.window_size.y) * 100.0f; // NOLINT(cppcoreguidelines-pro-type-union-access)
}

float server_content_height_px(std::size_t server_count) {
    if (server_count == 0)
        return 0.0f;
    return static_cast<float>(server_count) * (k_item_height_px + k_item_spacing_px) - k_item_spacing_px;
}

float clamp_scroll_offset(float offset_px, float max_scroll_px) {
    if (max_scroll_px <= 0.0f)
        return 0.0f;
    return std::clamp(offset_px, 0.0f, max_scroll_px);
}

bool parse_server_index(const std::string& tag_name, std::size_t& out_index) {
    if (tag_name.rfind("server_item_", 0) != 0)
        return false;
    const std::string k_index_str = tag_name.substr(k_tag_prefix_length);
    if (k_index_str.empty())
        return false;
    std::size_t value = 0;
    for (char c : k_index_str) {
        if (c < '0' || c > '9')
            return false;
        value = (value * k_digit_base_10) + static_cast<std::size_t>(c - '0');
    }
    out_index = value;
    return true;
}

std::string make_server_key(const std::string& ip, std::uint16_t port) {
    return ip + ":" + std::to_string(port);
}

void rebuild_server_keys(ServersState& state) {
    state.server_keys.clear();
    state.server_keys.reserve(state.servers.size());
    for (const auto& [key, server] : state.servers) {
        state.server_keys.push_back(key);
    }
    std::sort(state.server_keys.begin(), state.server_keys.end());
}

void clear_server_list_ui(EngineContext& ctx) {
    auto& tags = ctx.registry.get_tag_registry();
    auto& state = get_servers_state();
    for (size_t i = 0; i < state.server_item_tags.size(); i++) {
        auto tag_id = state.server_item_tags[i];
        if (tag_id == ecs::TagRegistry::k_invalid_tag_id)
            continue;
        auto ent_opt = tags.get_entity(tag_id);
        if (ent_opt.has_value()) {
            ctx.registry.kill_entity(ent_opt.value());
        }
    }
}

void update_server_list_ui(EngineContext& ctx) {
    clear_server_list_ui(ctx);

    auto& state = get_servers_state();
    rebuild_server_keys(state);
    const auto k_list_ent_opt = get_server_list_entity(ctx);
    if (!k_list_ent_opt.has_value())
        return;
    const auto* list_transform = get_server_list_transform(ctx);
    if (!list_transform)
        return;
    auto list_rect_opt = get_server_list_rect_px(ctx, *list_transform);
    if (!list_rect_opt)
        return;

    const UiRectPx k_list_rect = *list_rect_opt;
    const float k_clip_x_px = k_list_rect.x;
    const float k_clip_y_px = k_list_rect.y;
    const float k_clip_w_px = k_list_rect.w;
    const float k_clip_h_px = k_list_rect.h;

    auto& scroll_area = ensure_server_scroll_area(ctx, k_list_ent_opt.value());
    scroll_area.content_height_px = server_content_height_px(state.servers.size());

    const float k_viewport_height_px = std::max(0.0f, k_clip_h_px - (scroll_area.padding_px * 2.0f));
    scroll_area.scroll_offset_px =
        clamp_scroll_offset(scroll_area.scroll_offset_px,
                            std::max(0.0f, scroll_area.content_height_px - k_viewport_height_px));

    std::size_t server_count = state.server_keys.size();
    auto& tag_cache = state.server_item_tags;
    if (tag_cache.size() < server_count) {
        tag_cache.resize(server_count, ecs::TagRegistry::k_invalid_tag_id);
    }

    const float k_item_x_px = k_clip_x_px + scroll_area.padding_px;
    const float k_item_w_px = std::max(0.0f, k_clip_w_px - scroll_area.padding_px * 2.0f - k_scrollbar_width_px -
                                                 k_list_right_margin_px);
    const float k_start_y_px = k_clip_y_px + scroll_area.padding_px - scroll_area.scroll_offset_px;

    for (size_t i = 0; i < server_count; i++) {
        const auto& server_key = state.server_keys[i];
        const auto& server = state.servers.at(server_key);
        std::string server_tag = "server_item_" + std::to_string(i);

        auto& tag_registry = ctx.registry.get_tag_registry();
        auto entity = ctx.registry.spawn_entity();
        auto tag_id = tag_cache[i];
        if (tag_id != ecs::TagRegistry::k_invalid_tag_id) {
            tag_registry.bind_tag(tag_id, entity);
            ctx.registry.add_component(entity, cpnt::Tag{tag_id});
        } else {
            tag_id = tag_registry.create_and_bind_tag(server_tag, entity);
            if (tag_id == ecs::TagRegistry::k_invalid_tag_id) {
                ctx.registry.kill_entity(entity);
                continue;
            }
            tag_cache[i] = tag_id;
            ctx.registry.add_component(entity, cpnt::Tag{tag_id});
        }

        const float k_item_y_px = k_start_y_px + static_cast<float>(i) * (k_item_height_px + k_item_spacing_px);
        ctx.registry.add_component(
            entity, cpnt::UITransform{px_to_percent_x(ctx, k_item_x_px), px_to_percent_y(ctx, k_item_y_px), 0.0f,
                                      px_to_percent_x(ctx, k_item_w_px), px_to_percent_y(ctx, k_item_height_px), 0.0f,
                                      0.0f, 0.0f});

        std::string server_info = server.name + " - " + server.ip + ":" + std::to_string(server.port);
        if (state.selected_index.has_value() && state.selected_index.value() == i) {
            server_info = "> " + server_info;
        }
        ctx.registry.add_component(entity, cpnt::UIText{server_info, static_cast<int>(k_text_font_size)});

        const float k_clip_x = px_to_percent_x(ctx, k_clip_x_px);
        const float k_clip_y = px_to_percent_y(ctx, k_clip_y_px);
        const float k_clip_w = px_to_percent_x(ctx, k_clip_w_px);
        const float k_clip_h = px_to_percent_y(ctx, k_clip_h_px);
        ctx.registry.add_component(entity, cpnt::UIClip{k_clip_x, k_clip_y, k_clip_w, k_clip_h});
        ctx.registry.add_component(
            entity,
            cpnt::UIStyle{
                utils::Color{k_color_dark_bg, k_color_dark_bg, k_color_dark_bg, k_alpha_opaque},
                utils::Color{k_color_darker_bg, k_color_darker_bg, k_color_darker_bg, k_alpha_opaque},
                utils::Color{k_color_darker_pressed, k_color_darker_pressed, k_color_darker_pressed, k_alpha_opaque},
                utils::Color{k_color_light_text, k_color_light_text, k_color_light_text, k_alpha_opaque},
                utils::Color{k_color_white, k_color_white, k_color_white, k_alpha_opaque},
                utils::Color{k_color_light_gray, k_color_light_gray, k_color_light_gray, k_alpha_opaque},
                utils::Color{k_color_medium_gray, k_color_medium_gray, k_color_medium_gray, k_alpha_opaque},
                utils::Color{k_color_medium_light, k_color_medium_light, k_color_medium_light, k_alpha_opaque},
                utils::Color{k_color_medium, k_color_medium, k_color_medium, k_alpha_opaque},
                k_border_radius, k_border_thickness});

        ctx.registry.add_component(entity, cpnt::UIButton{});
        ctx.registry.add_component(entity, cpnt::UIInteractable{});
        ctx.registry.add_component(entity, cpnt::UIFocusable{false});
    }

    if (state.server_keys.empty()) {
        update_status_text(ctx, "No servers saved yet. Add one below.");
    }

    state.last_scroll_offset_px = scroll_area.scroll_offset_px;
    apply_server_list_navigation(ctx, server_count);
}

std::optional<std::uint16_t> parse_port(const std::string& value) {
    try {
        int port = std::stoi(value);
        if (port <= 0 || port > k_max_port_value)
            return std::nullopt;
        return static_cast<std::uint16_t>(port);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

void handle_add_server_button(EngineContext& ctx) {
    auto name_opt = get_text_for_tag(ctx, "server_name_input");
    auto ip_opt = get_text_for_tag(ctx, "server_ip_input");
    auto port_opt = get_text_for_tag(ctx, "server_port_input");

    if (!ip_opt.has_value() || ip_opt->empty()) {
        update_status_text(ctx, "Please enter a server IP");
        return;
    }
    if (!port_opt.has_value() || port_opt->empty()) {
        update_status_text(ctx, "Please enter a server port");
        return;
    }

    auto port = parse_port(*port_opt);
    if (!port.has_value()) {
        update_status_text(ctx, "Invalid port value");
        return;
    }

    std::string name = name_opt.value_or("");
    if (name.empty()) {
        name = *ip_opt + ":" + std::to_string(port.value());
    }

    auto& state = get_servers_state();
    bool updated = false;
    const std::string k_key = make_server_key(*ip_opt, port.value());
    if (auto it = state.servers.find(k_key); it != state.servers.end()) {
        it->second.name = name;
        updated = true;
    } else {
        state.servers.emplace(k_key, ServerEntry{name, *ip_opt, port.value()});
    }

    rebuild_server_keys(state);
    update_server_list_ui(ctx);
    update_status_text(ctx, updated ? "Server updated" : "Server added");
}

void handle_join_server_button(EngineContext& ctx) {
    auto& state = get_servers_state();
    if (state.connecting) {
        if (state.network_client) {
            state.network_client->disconnect();
            state.network_client.reset();
        }
        state.connecting = false;
        state.connect_elapsed_sec = 0.0f;
        state.pending_ip.clear();
        state.pending_port = 0;
        state.pending_status.reset();
        state.pending_login_success.reset();
    }

    std::optional<ServerEntry> selected;
    if (state.selected_index.has_value() && state.selected_index.value() < state.server_keys.size()) {
        selected = state.servers.at(state.server_keys[state.selected_index.value()]);
    }

    std::string ip;
    std::uint16_t port = 0;

    if (!selected.has_value()) {
        update_status_text(ctx, "Select a server before joining");
        return;
    }
    ip = selected->ip;
    port = selected->port;

    state.pending_ip = ip;
    state.pending_port = port;
    state.connecting = true;
    state.connect_elapsed_sec = 0.0f;

    state.network_client = std::make_shared<NetworkClient>();
    state.network_client->set_on_login([ip, port](bool success, uint32_t /*player_id*/) {
        auto& s = get_servers_state();
        std::lock_guard<std::mutex> lock(s.mutex);
        if (success) {
            s.pending_status = "Connected. Entering lobby...";
            s.pending_login_success = true;
        } else {
            s.pending_status = "Failed to connect to server";
            s.pending_login_success = false;
        }
        s.pending_ip = ip;
        s.pending_port = port;
    });

    try {
        state.network_client->connect(ip, port, "LobbyBrowser");
        update_status_text(ctx, "Connecting to " + ip + ":" + std::to_string(port) + "...");
    } catch (const std::exception& e) {
        state.connecting = false;
        state.network_client.reset();
        update_status_text(ctx, std::string("Connection error: ") + e.what());
    }
}

void handle_delete_server_button(EngineContext& ctx) {
    auto& state = get_servers_state();
    std::optional<std::size_t> index_to_remove;

    if (state.selected_index.has_value() && state.selected_index.value() < state.server_keys.size()) {
        index_to_remove = state.selected_index.value();
    } else {
        auto ip_opt = get_text_for_tag(ctx, "server_ip_input");
        auto port_opt = get_text_for_tag(ctx, "server_port_input");
        if (!ip_opt.has_value() || ip_opt->empty() || !port_opt.has_value() || port_opt->empty()) {
            update_status_text(ctx, "Select a server or enter IP and port to delete");
            return;
        }
        auto port_parsed = parse_port(*port_opt);
        if (!port_parsed.has_value()) {
            update_status_text(ctx, "Invalid port value");
            return;
        }
        const std::string k_key = make_server_key(*ip_opt, port_parsed.value());
        auto it = state.servers.find(k_key);
        if (it != state.servers.end()) {
            auto key_it = std::find(state.server_keys.begin(), state.server_keys.end(), k_key);
            if (key_it != state.server_keys.end())
                index_to_remove = static_cast<std::size_t>(std::distance(state.server_keys.begin(), key_it));
        }
    }

    if (!index_to_remove.has_value()) {
        update_status_text(ctx, "Server not found");
        return;
    }

    const std::string k_key = state.server_keys[index_to_remove.value()];
    state.servers.erase(k_key);
    state.selected_index.reset();
    state.pending_ip.clear();
    state.pending_port = 0;
    rebuild_server_keys(state);
    update_server_list_ui(ctx);
    update_status_text(ctx, "Server removed");
}

void handle_server_item_clicked(EngineContext& ctx, std::size_t index) {
    auto& state = get_servers_state();
    if (index >= state.server_keys.size())
        return;
    state.selected_index = index;

    const auto& server = state.servers.at(state.server_keys[index]);
    set_text_for_tag(ctx, "server_name_input", server.name);
    set_text_for_tag(ctx, "server_ip_input", server.ip);
    set_text_for_tag(ctx, "server_port_input", std::to_string(server.port));

    update_status_text(ctx, "Selected " + server.name);
    update_server_list_ui(ctx);
}

void update_server_list_on_scroll(EngineContext& ctx) {
    auto& state = get_servers_state();
    const auto k_list_ent_opt = get_server_list_entity(ctx);
    if (!k_list_ent_opt.has_value())
        return;
    auto& scroll_areas = ctx.registry.get_components<cpnt::UIScrollArea>();
    const std::size_t k_list_idx = k_list_ent_opt->value();
    if (k_list_idx >= scroll_areas.size() || !scroll_areas[k_list_idx].has_value())
        return;
    const float k_current_offset = scroll_areas[k_list_idx]->scroll_offset_px;
    if (std::abs(k_current_offset - state.last_scroll_offset_px) < k_scroll_epsilon_px)
        return;
    update_server_list_ui(ctx);
}

} // namespace

void reset_servers_ui_state() {
    auto& state = get_servers_state();
    rebuild_server_keys(state);
    state.server_item_tags.clear();
    state.selected_index.reset();
    state.last_scroll_offset_px = k_uninitialized_scroll_offset_px;
    state.status_message = "Select a server or add a new one";
    if (state.network_client) {
        state.network_client->disconnect();
    }
    state.network_client.reset();
    state.connecting = false;
    state.connect_elapsed_sec = 0.0f;
    state.pending_ip.clear();
    state.pending_port = 0;
    state.pending_status.reset();
    state.pending_login_success.reset();
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> evts_queue);

void handle_servers_ui_events(engn::EngineContext& engine_ctx) {
    auto& state = get_servers_state();
    if (state.last_scroll_offset_px == k_uninitialized_scroll_offset_px) {
        update_server_list_ui(engine_ctx);
        update_status_text(engine_ctx, state.status_message);
    }

    if (state.network_client && state.connecting) {
        state.connect_elapsed_sec += engine_ctx.delta_time;
        if (state.connect_elapsed_sec >= k_connect_timeout_sec) {
            state.network_client->disconnect();
            state.network_client.reset();
            state.connecting = false;
            state.connect_elapsed_sec = 0.0f;
            state.pending_ip.clear();
            state.pending_port = 0;
            state.pending_status.reset();
            state.pending_login_success.reset();
            update_status_text(engine_ctx, "Connection timed out");
            return;
        }
        state.network_client->poll();
        std::optional<std::string> pending_status;
        std::optional<bool> pending_login_success;
        std::string pending_ip;
        std::uint16_t pending_port = 0;
        {
            std::lock_guard<std::mutex> lock(state.mutex);
            pending_status = std::move(state.pending_status);
            pending_login_success = std::move(state.pending_login_success);
            pending_ip = state.pending_ip;
            pending_port = state.pending_port;
            state.pending_status.reset();
            state.pending_login_success.reset();
        }
        if (pending_status) {
            update_status_text(engine_ctx, *pending_status);
        }
        if (pending_login_success.has_value()) {
            state.connecting = false;
            state.connect_elapsed_sec = 0.0f;
            if (pending_login_success.value()) {
                attach_lobby_network_client(state.network_client, pending_ip, pending_port);
                state.network_client.reset();
                engine_ctx.set_scene("lobby");
                return;
            }
            if (state.network_client) {
                state.network_client->disconnect();
                state.network_client.reset();
            }
        }
    }

    update_server_list_on_scroll(engine_ctx);

    const auto& evts = engine_ctx.ui_event_queue;
    evts.for_each<evts::UIButtonClicked>(
        [&engine_ctx, &evts](const evts::UIButtonClicked& evt) { handle_ui_button_clicked(engine_ctx, evt, evts); });
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& e,
                                     engn::evts::EventQueue<engn::evts::UIEvent> /*evts_queue*/) {
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(e.tag);

    if (tag_name == "add_server_button") {
        handle_add_server_button(ctx);
    } else if (tag_name == "delete_server_button") {
        handle_delete_server_button(ctx);
    } else if (tag_name == "join_server_button") {
        handle_join_server_button(ctx);
    } else if (tag_name == "back_button") {
        auto& state = get_servers_state();
        if (state.network_client) {
            state.network_client->disconnect();
            state.network_client.reset();
        }
        state.connecting = false;
        ctx.set_scene("main_menu");
    } else if (tag_name.rfind("server_item_", 0) == 0) {
        std::size_t index = 0;
        if (parse_server_index(tag_name, index)) {
            handle_server_item_clicked(ctx, index);
        }
    }
}
