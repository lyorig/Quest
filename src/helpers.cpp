module;

#include <quest/game.hpp>

#include <halcyon/ttf.hpp>

#include <concepts>
#include <string_view>
#include <type_traits>

export module quest.helpers;

export namespace hq {
    // Find a font whose height of rendered text is the closest possible to the one requested.
    hal::font find_sized_font(game& g, std::string_view rel_path, hal::pixel_t desired_height) {
        constexpr hal::font::pt_t incr { 1 };

        hal::font       f;
        hal::font::pt_t curr { 4 };

        const std::string path { g.loader.resolve(rel_path) };

        do {
            f = g.ttf.make_font(path, curr);
            curr += incr;
        } while (f.render_solid("X", hal::colors::white).size().y < desired_height);

        return f;
    }

    // Get the size of a font and text.
    hal::pixel::point size_text(hal::ref<const hal::font> f, std::string_view text) {
        return hal::text { f, text }.size().get();
    }

    // An enum that has a `none` variant.
    template <typename T>
    concept has_none = std::is_enum_v<T> && requires {
        { T::none } -> std::same_as<T>;
    };

    // Returns `val` if `v == true`, otherwise returns `T::none`.
    template <has_none T>
    constexpr T cond_enum(T val, bool v) {
        return v ? val : T::none;
    }
}
