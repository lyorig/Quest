#include <quest/text_field.hpp>

using namespace HQ;

constexpr text_field::diff_t tab_spaces { 4 };

text_field::text_field()
    : cursor { 0 } { }

std::size_t text_field::process(std::string_view inp) {
    text.insert(cursor, inp);
    cursor += inp.size();
    return inp.size();
}

text_field::diff_t text_field::process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c) {
    switch (k) {
        using key = hal::keyboard::key;
        using mod = hal::keyboard::mod;

    case key::backspace:
        if (text.empty())
            break;

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
            cursor -= cursor - off;

            return static_cast<diff_t>(ret);

        } else { // delete one character
            if (cursor != 0) {
                --cursor;
            }

            text.erase(text.begin() + cursor);
            return 1;
        }

    case key::left_arrow:
        if (cursor != 0) {
            --cursor;
        }
        break;

    case key::right_arrow:
        cursor = std::min(static_cast<std::size_t>(cursor + 1), text.size());
        break;

    case key::tab:
        text.insert(cursor, tab_spaces, ' ');
        cursor += tab_spaces;
        return tab_spaces;

    case key::V:
        if (m[mod::ctrl] && c.has_text()) {
            const auto        str = c();
            const std::size_t sz { str.size() }; // one-time size calculation

            text.insert(cursor, str.c_str(), sz);
            cursor += sz;

            return static_cast<diff_t>(sz);
        }
        break;

    default:
        break;
    }

    if (cursor > text.size())
        HAL_PRINT("<Text Field> Oops, cursor is OOB: ", cursor);

    return 0; // Nothing changed.
}