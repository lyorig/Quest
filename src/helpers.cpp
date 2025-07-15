#include <quest/helpers.hpp>

#include <halcyon/ttf.hpp>

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
    } while (f.render(TestChar)().size().y < desired_height);

    return f;
}

hal::pixel::point hq::size_text(hal::ref<const hal::font> f, std::string_view text) {
    return hal::text { f, text }.size().get();
}
