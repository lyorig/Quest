#include <quest/text_field.hpp>

using namespace HQ;

constexpr text_field::diff_t tab_spaces { 4 };

std::size_t text_field::process(std::string_view inp) {
    text.append(inp);
    return inp.size();
}

text_field::diff_t text_field::process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c) {
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

                const std::size_t ret { text.size() - off };

                text.erase(text.begin() + off, text.end());

                return static_cast<diff_t>(ret);

            } else { // delete one character
                text.pop_back();
                return 1;
            }
        }
        break;

    case key::tab:
        text.append(tab_spaces, ' ');

        return tab_spaces;

    case key::V:
        if (m[mod::ctrl] && c.has_text()) {
            const auto        str = c();
            const std::size_t sz { str.size() }; // one-time size calculation

            text += std::string_view { str.c_str(), sz };

            return static_cast<diff_t>(sz);
        }

    default:
        break;
    }

    return 0; // Nothing changed.
}