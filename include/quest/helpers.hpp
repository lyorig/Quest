#include <halcyon/ttf.hpp>

namespace hq {
    // Find a font whose height of rendered text is the closest possible to the one requested.
    hal::font find_sized_font(hal::ttf::context& ttf, std::string_view path, hal::pixel_t desired_height);

    template <typename T>
        requires std::is_enum_v<T>
    constexpr T cond_enum(T val, bool v) {
        return v ? val : T::none;
    }
}