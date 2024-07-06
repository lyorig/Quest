#include <quest/field.hpp>

using namespace hq;

namespace consts {
    constexpr std::size_t tab_size { 4 };
}

field::field()
    : cursor { 0 } { }

bool field::process(std::string_view inp) {
    text.insert(cursor, inp);
    cursor += inp.size();

    for (const char ch : inp) {
        if (!std::isspace(ch)) {
            return true;
        }
    }

    return false;
}

field::op field::process(hal::keyboard::key k, const hal::proxy::video& vid) {
    switch (k) {
        using key = hal::keyboard::key;
        using mod = hal::keyboard::mod;

    case key::backspace:
        if (text.empty())
            break;

        if (vid.events.keyboard.mod()[mod::ctrl]) {
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

                cursor -= end - begin;
            }

            text.erase(text.begin() + begin, text.begin() + end);

            return op::text_removed;

        } else { // delete one character
            if (cursor != 0) {
                --cursor;
            }

            text.erase(text.begin() + cursor);

            return op::text_removed;
        }

    case key::left_arrow:
        if (cursor != 0) {
            --cursor;
        }

        return op::cursor_moved;

    case key::right_arrow:
        cursor = std::min(static_cast<std::size_t>(cursor + 1), text.size());

        return op::cursor_moved;

    case key::tab:
        text.insert(cursor, consts::tab_size, ' ');
        cursor += consts::tab_size;

        return op::text_added;

    case key::V:
        if (vid.events.keyboard.mod()[mod::ctrl] && vid.clipboard.has_text()) {
            const auto        str = vid.clipboard.text();
            const std::size_t sz { str.size() }; // one-time size calculation

            text.insert(cursor, str.c_str(), sz);
            cursor += sz;

            return op::text_added;
        }

        break;

    default:
        break;
    }

    HAL_WARN_IF(cursor > text.size(), "<Text Field> Oops, cursor is OOB: ", cursor);

    return op::nothing; // Nothing changed.
}

void field::trim(std::size_t off) {
    if (cursor > off) {
        cursor -= cursor - off;
    }

    text.erase(off);
}