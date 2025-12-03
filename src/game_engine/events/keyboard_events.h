#pragma once

namespace engn {

namespace evts {

enum KeyboardKeyCode {
    key_unknown = -1,

    key_a = 0,
    key_b,
    key_c,
    key_d,
    key_e,
    key_f,
    key_g,
    key_h,
    key_i,
    key_j,
    key_k,
    key_l,
    key_m,
    key_n,
    key_o,
    key_p,
    key_q,
    key_r,
    key_s,
    key_t,
    key_u,
    key_v,
    key_w,
    key_x,
    key_y,
    key_z,

    key_0,
    key_1,
    key_2,
    key_3,
    key_4,
    key_5,
    key_6,
    key_7,
    key_8,
    key_9,

    key_escape,
    key_enter,
    key_tab,
    key_backspace,
    key_insert,
    key_delete,

    key_right,
    key_left,
    key_down,
    key_up,

    key_page_up,
    key_page_down,

    key_home,
    key_end,

    key_caps_lock,
    key_scroll_lock,
    key_num_lock,
    key_print_screen,
    key_pause,

    key_f1,
    key_f2,
    key_f3,
    key_f4,
    key_f5,
    key_f6,
    key_f7,
    key_f8,
    key_f9,
    key_f10,
    key_f11,
    key_f12,

    key_space,

    key_left_shift,
    key_right_shift,

    key_left_control,
    key_right_control,

    key_left_alt,
    key_right_alt,

    key_left_super,
    key_right_super
};

struct KeyPressed {
    KeyboardKeyCode keycode;
};

struct KeyHold {
    KeyboardKeyCode keycode;
};

struct KeyReleased {
    KeyboardKeyCode keycode;
};

} // namespace events

} // namespace engine
