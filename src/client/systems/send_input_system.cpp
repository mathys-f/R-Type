#include "client_systems.h"
#include "game_engine/engine.h"
#include "network_client.h"
#include "game_engine/events/events.h"
#include "utils/logger.h"

#include <cstdint>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static NetworkClient* g_s_client = nullptr;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

constexpr std::uint8_t INPUT_MASK_UP = 0x01;
constexpr std::uint8_t INPUT_MASK_DOWN = 0x02;
constexpr std::uint8_t INPUT_MASK_LEFT = 0x04;
constexpr std::uint8_t INPUT_MASK_RIGHT = 0x08;
constexpr std::uint8_t INPUT_MASK_SHOOT = 0x10;

void set_network_client(void* client_ptr) {
    g_s_client = static_cast<NetworkClient*>(client_ptr);
}

void send_input_system(engn::EngineContext& ctx) {
    // Send input every game tick
    bool up = false, down = false, left = false, right = false, shoot = false;

    auto check_key_in_queue = [&](engn::evts::KeyboardKeyCode key) {
        bool found = false;
        ctx.input_event_queue.for_each<engn::evts::KeyHold>([&](const engn::evts::KeyHold& e) {
            if (e.keycode == key) found = true;
        });
        if (found) return true;
        ctx.input_event_queue.for_each<engn::evts::KeyPressed>([&](const engn::evts::KeyPressed& e) {
            if (e.keycode == key) found = true;
        });
        return found;
    };

    const auto& c = ctx.controls;
    if (check_key_in_queue(c.move_up.primary) || check_key_in_queue(c.move_up.secondary)) up = true;
    if (check_key_in_queue(c.move_down.primary) || check_key_in_queue(c.move_down.secondary)) down = true;
    if (check_key_in_queue(c.move_left.primary) || check_key_in_queue(c.move_left.secondary)) left = true;
    if (check_key_in_queue(c.move_right.primary) || check_key_in_queue(c.move_right.secondary)) right = true;
    if (check_key_in_queue(c.shoot.primary) || check_key_in_queue(c.shoot.secondary)) shoot = true;

    std::uint8_t mask = 0;
    mask |= static_cast<std::uint8_t>(up ? INPUT_MASK_UP : 0x00);
    mask |= static_cast<std::uint8_t>(down ? INPUT_MASK_DOWN : 0x00);
    mask |= static_cast<std::uint8_t>(left ? INPUT_MASK_LEFT : 0x00);
    mask |= static_cast<std::uint8_t>(right ? INPUT_MASK_RIGHT : 0x00);
    mask |= static_cast<std::uint8_t>(shoot ? INPUT_MASK_SHOOT : 0x00);

    if (g_s_client && g_s_client->is_connected()) {
        g_s_client->send_input_mask(mask, static_cast<std::uint32_t>(ctx.get_current_tick()));
    }
}
