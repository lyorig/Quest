#include <quest/game.hpp>
#include <quest/helpers.hpp>

#include <halcyon/ttf.hpp>

hal::font hq::find_sized_font(game& g, std::string_view rel_path, hal::pixel_t desired_height) {
    constexpr hal::font::pt_t incr { 1 };

    hal::font       f;
    hal::font::pt_t curr { 4 };

    const std::string path { g.loader.resolve(rel_path) };

    do {
        f = g.ttf.load(path, curr);
        curr += incr;
    } while (f.render("X")().size().y < desired_height);

    return f;
}

hal::pixel::point hq::size_text(hal::ref<const hal::font> f, std::string_view text) {
    return hal::text { f, text }.size().get();
}
