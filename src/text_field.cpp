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
    m_text.push_back(ch);
}

bool text_field::process(hal::keyboard::key k, hal::keyboard::mod_state m) {
    switch (k) {
        using key = hal::keyboard::key;
        using mod = hal::keyboard::mod;

    case key::backspace:
        if (!m_text.empty()) {
            if (m[mod::ctrl]) { // delete entire word
                std::size_t off;

                if (m_text.back() == ' ') {
                    off = m_text.find_last_not_of(' ') + 1;
                } else {
                    off = m_text.find_last_of(' ');
                    if (off == std::string::npos)
                        off = 0;
                }

                m_text.erase(m_text.begin() + off, m_text.end());

            } else { // delete one character
                m_text.pop_back();
            }

            return true;
        }
        break;

    case key::tab:
        m_text.append(4, ' ');
        return true;

    default:
        break;
    }

    return false;
}

bool text_field::has_focus() const {
    return m_focus;
}

const std::string& text_field::text() const {
    return m_text;
}