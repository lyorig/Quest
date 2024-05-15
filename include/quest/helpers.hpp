#include <halcyon/ttf.hpp>

namespace HQ {
    hal::font find_sized_font(hal::ttf::context& ttf, std::string_view path, hal::pixel_t desired_height);
}