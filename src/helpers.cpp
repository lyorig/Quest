#include <quest/helpers.hpp>

hal::font HQ::find_sized_font(hal::ttf::context& ttf, std::string_view path, hal::pixel_t desired_height) {
    constexpr hal::font::pt_t incr { 1 };

    hal::font       f;
    hal::font::pt_t curr { 0 };

    while ((f = ttf.load(path, curr += incr)).size_text("A").y < desired_height)
        ;

    return f;
}