#pragma once

#include <halcyon/video/types.hpp>

namespace HQ::consts {
    constexpr hal::pixel_t renderer_height { 768 };

    constexpr hal::coord_t meters_per_pixel { 0.2 };
    constexpr hal::coord_t pixels_per_meter { 1.0 / meters_per_pixel };
}