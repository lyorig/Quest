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

        if (m[mod::ctrl]) {
            std::size_t begin, end;

            if (cursor == 0) { // delete from beginning
                begin = end = 0;

                const char curr { text[begin] };

                if (curr == ' ') { // delete spaces
                    for (; end != text.size() && text[end] == ' '; ++end)
                        ;
                } else if (std::isalpha(curr)) { // delete letters
                    for (; end != text.size() && std::isalpha(text[end]); ++end)
                        ;
                } else { // delete... not letters?
                    for (; end != text.size() && !std::isalpha(text[end]) && text[end] != ' '; ++end)
                        ;
                }
            }

            else { // delete from cursor
                begin = cursor - 1;
                end   = cursor;

                const char curr { text[begin] };

                if (curr == ' ') { // delete spaces
                    for (; begin != 0 && text[begin] == ' '; --begin)
                        ;
                } else if (std::isalpha(curr)) { // delete letters
                    for (; begin != 0 && std::isalpha(text[begin]); --begin)
                        ;
                } else { // delete... not letters?
                    for (; begin != 0 && !std::isalpha(text[begin]) && text[begin] != ' '; --begin)
                        ;

                    if (begin != 0)
                        ++begin;
                }
            }

            HAL_PRINT("<Text Field> Offset ", begin);

            text.erase(text.begin() + begin, text.begin() + end);
            cursor -= end - begin;

            return true;

        } else { // delete one character
            if (cursor != 0) {
                --cursor;
            }

            text.erase(text.begin() + cursor);
            return true;
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