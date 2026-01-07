#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "game_engine/controls.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
// Explosion constants
constexpr float k_explosion_sprite_y = 99.0f;
constexpr float k_explosion_sprite_w = 65.0f;
constexpr float k_explosion_sprite_h = 64.0f;
constexpr float k_explosion_scale = 2.0f;
constexpr float k_explosion_frame_duration = 0.08f;
constexpr int k_explosion_frames = 5;

// Player movement
constexpr float k_player_speed = 400.0f;

// Player bounds
constexpr float k_player_width = 99.0f;
constexpr float k_player_height = 54.0f;

// Animation
constexpr int k_animation_frame_limit = 5;
constexpr float k_sprite_down_left = 100.0f;
constexpr float k_sprite_down_anim = 133.0f;
constexpr float k_sprite_neutral = 166.0f;
constexpr float k_sprite_up_anim = 199.0f;
constexpr float k_sprite_up_right = 232.0f;

// Rotation
constexpr float k_rotation_speed = 5.0f;

// Bullet
constexpr float k_bullet_offset_x = 50.0f;
constexpr float k_bullet_offset_y = 30.0f;
constexpr float k_bullet_speed = 650.0f;
constexpr float k_bullet_width = 16.0f;
constexpr float k_bullet_height = 8.0f;
constexpr float k_bullet_sprite_x = 249.0f;
constexpr float k_bullet_sprite_y = 105.0f;
constexpr float k_bullet_scale = 2.0f;
} // namespace

static int keycode_to_raylib(evts::KeyboardKeyCode keycode) {
    switch (keycode) {
        case evts::KeyboardKeyCode::KeyA: return KEY_A;
        case evts::KeyboardKeyCode::KeyB: return KEY_B;
        case evts::KeyboardKeyCode::KeyC: return KEY_C;
        case evts::KeyboardKeyCode::KeyD: return KEY_D;
        case evts::KeyboardKeyCode::KeyE: return KEY_E;
        case evts::KeyboardKeyCode::KeyF: return KEY_F;
        case evts::KeyboardKeyCode::KeyG: return KEY_G;
        case evts::KeyboardKeyCode::KeyH: return KEY_H;
        case evts::KeyboardKeyCode::KeyI: return KEY_I;
        case evts::KeyboardKeyCode::KeyJ: return KEY_J;
        case evts::KeyboardKeyCode::KeyK: return KEY_K;
        case evts::KeyboardKeyCode::KeyL: return KEY_L;
        case evts::KeyboardKeyCode::KeyM: return KEY_M;
        case evts::KeyboardKeyCode::KeyN: return KEY_N;
        case evts::KeyboardKeyCode::KeyO: return KEY_O;
        case evts::KeyboardKeyCode::KeyP: return KEY_P;
        case evts::KeyboardKeyCode::KeyQ: return KEY_Q;
        case evts::KeyboardKeyCode::KeyR: return KEY_R;
        case evts::KeyboardKeyCode::KeyS: return KEY_S;
        case evts::KeyboardKeyCode::KeyT: return KEY_T;
        case evts::KeyboardKeyCode::KeyU: return KEY_U;
        case evts::KeyboardKeyCode::KeyV: return KEY_V;
        case evts::KeyboardKeyCode::KeyW: return KEY_W;
        case evts::KeyboardKeyCode::KeyX: return KEY_X;
        case evts::KeyboardKeyCode::KeyY: return KEY_Y;
        case evts::KeyboardKeyCode::KeyZ: return KEY_Z;
        case evts::KeyboardKeyCode::Key0: return KEY_ZERO;
        case evts::KeyboardKeyCode::Key1: return KEY_ONE;
        case evts::KeyboardKeyCode::Key2: return KEY_TWO;
        case evts::KeyboardKeyCode::Key3: return KEY_THREE;
        case evts::KeyboardKeyCode::Key4: return KEY_FOUR;
        case evts::KeyboardKeyCode::Key5: return KEY_FIVE;
        case evts::KeyboardKeyCode::Key6: return KEY_SIX;
        case evts::KeyboardKeyCode::Key7: return KEY_SEVEN;
        case evts::KeyboardKeyCode::Key8: return KEY_EIGHT;
        case evts::KeyboardKeyCode::Key9: return KEY_NINE;
        case evts::KeyboardKeyCode::KeyEscape: return KEY_ESCAPE;
        case evts::KeyboardKeyCode::KeyEnter: return KEY_ENTER;
        case evts::KeyboardKeyCode::KeyTab: return KEY_TAB;
        case evts::KeyboardKeyCode::KeyBackspace: return KEY_BACKSPACE;
        case evts::KeyboardKeyCode::KeyInsert: return KEY_INSERT;
        case evts::KeyboardKeyCode::KeyDelete: return KEY_DELETE;
        case evts::KeyboardKeyCode::KeyPeriod: return KEY_PERIOD;
        case evts::KeyboardKeyCode::KeyRight: return KEY_RIGHT;
        case evts::KeyboardKeyCode::KeyLeft: return KEY_LEFT;
        case evts::KeyboardKeyCode::KeyDown: return KEY_DOWN;
        case evts::KeyboardKeyCode::KeyUp: return KEY_UP;
        case evts::KeyboardKeyCode::KeyPageUp: return KEY_PAGE_UP;
        case evts::KeyboardKeyCode::KeyPageDown: return KEY_PAGE_DOWN;
        case evts::KeyboardKeyCode::KeyHome: return KEY_HOME;
        case evts::KeyboardKeyCode::KeyEnd: return KEY_END;
        case evts::KeyboardKeyCode::KeyCapsLock: return KEY_CAPS_LOCK;
        case evts::KeyboardKeyCode::KeyScrollLock: return KEY_SCROLL_LOCK;
        case evts::KeyboardKeyCode::KeyNumLock: return KEY_NUM_LOCK;
        case evts::KeyboardKeyCode::KeyPrintScreen: return KEY_PRINT_SCREEN;
        case evts::KeyboardKeyCode::KeyPause: return KEY_PAUSE;
        case evts::KeyboardKeyCode::KeyF1: return KEY_F1;
        case evts::KeyboardKeyCode::KeyF2: return KEY_F2;
        case evts::KeyboardKeyCode::KeyF3: return KEY_F3;
        case evts::KeyboardKeyCode::KeyF4: return KEY_F4;
        case evts::KeyboardKeyCode::KeyF5: return KEY_F5;
        case evts::KeyboardKeyCode::KeyF6: return KEY_F6;
        case evts::KeyboardKeyCode::KeyF7: return KEY_F7;
        case evts::KeyboardKeyCode::KeyF8: return KEY_F8;
        case evts::KeyboardKeyCode::KeyF9: return KEY_F9;
        case evts::KeyboardKeyCode::KeyF10: return KEY_F10;
        case evts::KeyboardKeyCode::KeyF11: return KEY_F11;
        case evts::KeyboardKeyCode::KeyF12: return KEY_F12;
        case evts::KeyboardKeyCode::KeySpace: return KEY_SPACE;
        case evts::KeyboardKeyCode::KeyLeftShift: return KEY_LEFT_SHIFT;
        case evts::KeyboardKeyCode::KeyRightShift: return KEY_RIGHT_SHIFT;
        case evts::KeyboardKeyCode::KeyLeftControl: return KEY_LEFT_CONTROL;
        case evts::KeyboardKeyCode::KeyRightControl: return KEY_RIGHT_CONTROL;
        case evts::KeyboardKeyCode::KeyLeftAlt: return KEY_LEFT_ALT;
        case evts::KeyboardKeyCode::KeyRightAlt: return KEY_RIGHT_ALT;
        case evts::KeyboardKeyCode::KeyLeftSuper: return KEY_LEFT_SUPER;
        case evts::KeyboardKeyCode::KeyRightSuper: return KEY_RIGHT_SUPER;
        case evts::KeyboardKeyCode::KeyUnknown: return KEY_NULL;
    }
    return KEY_NULL;
}

static bool is_key_down(evts::KeyboardKeyCode keycode) {
    const int k_key = keycode_to_raylib(keycode);
    return k_key != KEY_NULL && IsKeyDown(k_key);
}

static bool is_key_pressed(evts::KeyboardKeyCode keycode) {
    const int k_key = keycode_to_raylib(keycode);
    return k_key != KEY_NULL && IsKeyPressed(k_key);
}

static bool is_binding_down(const ControlBinding& binding) {
    return is_key_down(binding.primary) || is_key_down(binding.secondary);
}

static bool is_binding_pressed(const ControlBinding& binding) {
    return is_key_pressed(binding.primary) || is_key_pressed(binding.secondary);
}

void sys::player_control_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                ecs::SparseArray<cpnt::Player> const& players,
                                ecs::SparseArray<cpnt::Sprite> const& sprites,
                                ecs::SparseArray<cpnt::Velocity> const& velocities,
                                ecs::SparseArray<cpnt::Health> const& healths) {
    std::vector<ecs::Entity> to_remove;
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, player_opt, vel_opt, hp_opt] :
         ecs::indexed_zipper(positions, players, velocities, healths)) {
        // Handle player health
        auto& hp = reg.get_components<cpnt::Health>()[idx];
        auto& pos = reg.get_components<cpnt::Transform>()[idx];

        if (hp && hp->hp <= 0) {
            auto explosion = reg.spawn_entity();
            reg.add_component(explosion, cpnt::Transform{pos->x, pos->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
            reg.add_component(explosion,
                              cpnt::Sprite{{0.0f, k_explosion_sprite_y, k_explosion_sprite_w, k_explosion_sprite_h},
                                           k_explosion_scale,
                                           0,
                                           "explosion"});
            reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
            reg.add_component(explosion, cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                                         k_explosion_frame_duration, 0, k_explosion_frames});
            to_remove.push_back(reg.entity_from_index(idx));
            continue;
        }

        if (pos_opt && player_opt) {
            auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];

            if (pos) {
                if (is_binding_down(ctx.controls.move_right))
                    pos->x += k_player_speed * dt;
                if (is_binding_down(ctx.controls.move_left))
                    pos->x -= k_player_speed * dt;
                if (is_binding_down(ctx.controls.move_up))
                    pos->y -= k_player_speed * dt;
                if (is_binding_down(ctx.controls.move_down))
                    pos->y += k_player_speed * dt;

                // Clamp position
                // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                if (pos->x >= ctx.k_window_size.x - k_player_width)
                    pos->x = ctx.k_window_size.x - k_player_width;
                if (pos->x <= 0)
                    pos->x = 0;
                if (pos->y >= ctx.k_window_size.y - k_player_height)
                    pos->y = ctx.k_window_size.y - k_player_height;
                if (pos->y <= 0)
                    pos->y = 0;
                // NOLINTEND(cppcoreguidelines-pro-type-union-access)

                // Animate sprite
                if (sprite) {
                    if (is_binding_down(ctx.controls.move_up)) {
                        if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_up_right) {
                            sprite->frame++;
                            sprite->source_rect.x = k_sprite_up_anim;
                        } else {
                            sprite->source_rect.x = k_sprite_up_right;
                            sprite->frame = 0;
                        }
                    } else if (is_binding_down(ctx.controls.move_down)) {
                        if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_down_left) {
                            sprite->frame++;
                            sprite->source_rect.x = k_sprite_down_anim;
                        } else {
                            sprite->source_rect.x = k_sprite_down_left;
                            sprite->frame = 0;
                        }
                    } else {
                        if (sprite->frame <= k_animation_frame_limit && (sprite->source_rect.x == k_sprite_down_left ||
                                                                         sprite->source_rect.x == k_sprite_down_anim)) {
                            sprite->frame++;
                            sprite->source_rect.x = k_sprite_down_anim;
                        } else if (sprite->frame <= k_animation_frame_limit &&
                                   (sprite->source_rect.x == k_sprite_up_right ||
                                    sprite->source_rect.x == k_sprite_up_anim)) {
                            sprite->frame++;
                            sprite->source_rect.x = k_sprite_up_anim;
                        } else {
                            sprite->source_rect.x = k_sprite_neutral;
                            sprite->frame = 0;
                        }
                    }
                    if (is_binding_down(ctx.controls.move_left)) {
                        vel->vz = -k_rotation_speed;
                    } else if (is_binding_down(ctx.controls.move_right)) {
                        vel->vz = k_rotation_speed;
                    } else {
                        vel->vz = 0.0f;
                    }
                }

                // Rotation based on movement
                if (vel) {
                    if (is_binding_down(ctx.controls.move_left)) {
                        vel->vrz = -k_rotation_speed;
                    } else if (is_binding_down(ctx.controls.move_right)) {
                        vel->vrz = k_rotation_speed;
                    } else {
                        vel->vrz = 0.0f;
                    }
                }

                // Shoot
                if (is_binding_pressed(ctx.controls.shoot)) {
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Transform{pos->x + k_bullet_offset_x, pos->y + k_bullet_offset_y,
                                                              0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                    reg.add_component(bullet, cpnt::Velocity{k_bullet_speed, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::Bullet{});
                    reg.add_component(bullet, cpnt::Hitbox{0.0f, 0.0f, k_bullet_width, k_bullet_height});
                    reg.add_component(
                        bullet, cpnt::Sprite{{k_bullet_sprite_x, k_bullet_sprite_y, k_bullet_width, k_bullet_height},
                                             k_bullet_scale,
                                             0,
                                             "player_ship"});
                    std::optional<Sound> shoot_sound = ctx.assets_manager.get_asset<Sound>("shoot_sound");
                    if (shoot_sound.has_value())
                        PlaySound(shoot_sound.value());
                }
            }
        }
    }

    for (auto e : to_remove) {
        reg.kill_entity(e);
    }
}
