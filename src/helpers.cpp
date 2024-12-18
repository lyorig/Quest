#include <quest/helpers.hpp>

namespace {
    consteval bool is_upper(char c) {
        return c >= 'A' && c <= 'Z';
    }
}

hal::font hq::find_sized_font(hal::ttf::context& ttf, hal::c_string path, hal::pixel_t desired_height) {
    constexpr hal::font::pt_t incr { 1 };
    constexpr hal::c_string   TestChar { "X" };

    static_assert(TestChar.length() == 1 && is_upper(TestChar[0]));

    hal::font       f;
    hal::font::pt_t curr { 4 };

    do {
        f = ttf.load(path, curr);
        curr += incr;
    } while (f.size_text(TestChar).y < desired_height);

    return f;
}
