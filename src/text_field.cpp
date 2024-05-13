#include <quest/text_field.hpp>

#include <halcyon/events.hpp>

using namespace HQ;

text_field::text_field()
    : m_focus { false } {
}

bool text_field::toggle() {
    m_focus = !m_focus;
    return has_focus();
}

void text_field::process(char ch) {
    text.push_back(ch);
}

text_field::op text_field::process(hal::keyboard::key k, hal::keyboard::mod_state m) {
    switch (k) {
        using key = hal::keyboard::key;
        using mod = hal::keyboard::mod;

    case key::backspace:
        if (!text.empty()) {
            if (m[mod::ctrl]) { // delete entire word
                std::size_t off;

                if (text.back() == ' ') {
                    off = text.find_last_not_of(' ') + 1;
                } else {
                    off = text.find_last_of(' ');
                    if (off == std::string::npos)
                        off = 0;
                }

                text.erase(text.begin() + off, text.end());

            } else { // delete one character
                text.pop_back();
            }

            return op::remove;
        }
        break;

    case key::tab:
        text.append(4, ' ');
        return op::add;

    default:
        break;
    }

    return op::nothing;
}

bool text_field::has_focus() const {
    return m_focus;
}