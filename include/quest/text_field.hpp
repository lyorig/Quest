#pragma once

#include <string>

#include <halcyon/video.hpp>

// text_field:
// A text field manipulated via keystrokes.

namespace HQ {
    class text_field {
    public:
        // The operation that took place.
        // The LSB signifies if anything happened.
        enum class op : hal::u8 {
            nothing = 0b00,
            add     = 0b01,
            remove  = 0b11
        };

        text_field();

        // Returns the new state.
        bool toggle();

        // Process a character. Assume that changes are always made.
        void process(char ch);

        // Process a key. Returns the amount of characters added/removed.
        op process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c);

        bool has_focus() const;

        std::string text;

    private:
        bool m_focus;
    };
}