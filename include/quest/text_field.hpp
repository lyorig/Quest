#pragma once

#include <string>

#include <halcyon/video.hpp>

// text_field:
// A text field manipulated via keystrokes.

namespace HQ {
    class text_field {
    public:
        using diff_t = hal::i8;

        text_field();

        void show();
        void hide();

        // Returns the new state.
        bool toggle();

        // Process a character. Assume that changes are always made.
        void process(char ch);

        // Process a key. Returns the amount of characters added/removed.
        diff_t process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c);

        bool has_focus() const;

        std::string text;

    private:
        bool m_focus;
    };
}