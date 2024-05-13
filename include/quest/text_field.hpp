#pragma once

#include <string>

#include <halcyon/events/keyboard.hpp>

// text_field:
// A text field manipulated via keystrokes.

namespace HQ {
    class text_field {
    public:
        enum class op : hal::u8 {
            nothing,
            add,
            remove
        };

        text_field();

        // Returns the new state.
        bool toggle();

        // Process a character. Assume that changes are always made.
        void process(char ch);

        // Process a key. Returns the amount of characters added/removed.
        op process(hal::keyboard::key k, hal::keyboard::mod_state m);

        bool               has_focus() const;
        const std::string& text() const;

    private:
        std::string m_text;

        bool m_focus;
    };
}