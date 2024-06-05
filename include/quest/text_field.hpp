#pragma once

#include <string>

#include <halcyon/video.hpp>

// text_field:
// A text field manipulated via keystrokes.

namespace HQ {
    class text_field {
    public:
        text_field();

        // Process some text. Returns whether visual changes have been made.
        bool process(std::string_view inp);

        // Process a key. Returns the amount of characters added/removed.
        bool process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c);

        std::string text;
        std::size_t cursor;
    };
}