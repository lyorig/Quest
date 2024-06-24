#pragma once

#include <string>

#include <halcyon/video.hpp>

// field:
// A text field manipulated via keystrokes.

namespace hq {
    class field {
    public:
        enum class op {
            nothing,
            text_added,
            text_removed,
            cursor_moved
        };

        field();

        // Process some text. Returns whether visual changes have been made.
        bool process(std::string_view inp);

        // Process a key. Returns what happened to the field.
        op process(hal::keyboard::key k, const hal::proxy::video& vid);

        // Trim the field from [off] to the end.
        void trim(std::size_t off);

        std::string text;
        std::size_t cursor;
    };
}