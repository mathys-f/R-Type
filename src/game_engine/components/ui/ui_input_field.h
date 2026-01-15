#pragma once

#include <string>

namespace engn {

namespace cpnt {

// If present, marks a text attached to the same entity as an input field
struct UIInputField {
    bool editing = false; // Is the input field currently being edited
    float timer{};        // Used to time the cursor blink
    std::size_t cursor_index = 0; // Cursor position in the text
};

} // namespace cpnt

} // namespace engn
