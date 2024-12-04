#include <halcyon/ttf.hpp>

namespace hq {
    // Find a font whose height of rendered text is the closest possible to the one requested.
    hal::font find_sized_font(hal::ttf::context& ttf, hal::c_string path, hal::pixel_t desired_height);

    // An enum that has a "none" variant.
    template <typename T>
    concept has_none = std::is_enum_v<T> && requires {
        { T::none } -> std::same_as<T>;
    };

    // Return the [val] enum if [v] is true, else the enum's "none" value.
    template <has_none T>
    constexpr T cond_enum(T val, bool v) {
        return v ? val : T::none;
    }
}
