#include <quest/text_field.hpp>

using namespace HQ;

text_field::text_field()
    : cursor { 0 } { }

void text_field::process(std::string_view inp) {
    text.insert(cursor, inp);
    cursor += inp.size();
}

bool text_field::process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c) {
    switch (k) {
        using key = hal::keyboard::key;
        using mod = hal::keyboard::mod;

    case key::backspace:
        if (text.empty())
            break;

        if (m[mod::ctrl]) { // delete entire word
            if (cursor == 0) {
            }
            std::size_t off;

            if (text.back() == ' ') {
                off = text.find_last_not_of(' ', cursor) + 1;
            } else {
                off = text.find_last_of(' ', cursor);
                if (off == std::string::npos)
                    off = 0;
            }

            const std::size_t ret { text.size() - off };

            text.erase(text.begin() + off, text.end());
            cursor -= cursor - off;

            return true;

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
        text.insert(cursor, 4, ' ');
        cursor += 4;
        return true;

    case key::V:
        if (m[mod::ctrl] && c.has_text()) {
            const auto        str = c.text();
            const std::size_t sz { str.size() }; // one-time size calculation

            text.insert(cursor, str.c_str(), sz);
            cursor += sz;

            return true;
        }
        break;

    default:
        break;
    }

    if (cursor > text.size())
        HAL_PRINT("<Text Field> Oops, cursor is OOB: ", cursor);

    return false; // Nothing changed.
}