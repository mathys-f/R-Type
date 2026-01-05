#pragma once

namespace engn {

namespace evts {

enum class KeyboardKeyCode {
    KeyUnknown = 0,

    KeyA,
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    KeyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,

    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,

    KeyEscape,
    KeyEnter,
    KeyTab,
    KeyBackspace,
    KeyInsert,
    KeyDelete,

    KeyPeriod, // '.'

    KeyRight,
    KeyLeft,
    KeyDown,
    KeyUp,

    KeyPageUp,
    KeyPageDown,

    KeyHome,
    KeyEnd,

    KeyCapsLock,
    KeyScrollLock,
    KeyNumLock,
    KeyPrintScreen,
    KeyPause,

    KeyF1,
    KeyF2,
    KeyF3,
    KeyF4,
    KeyF5,
    KeyF6,
    KeyF7,
    KeyF8,
    KeyF9,
    KeyF10,
    KeyF11,
    KeyF12,

    KeySpace,

    KeyLeftShift,
    KeyRightShift,

    KeyLeftControl,
    KeyRightControl,

    KeyLeftAlt,
    KeyRightAlt,

    KeyLeftSuper,
    KeyRightSuper
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

} // namespace evts

} // namespace engn
