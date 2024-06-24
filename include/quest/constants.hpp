#pragma once

#include <halcyon/video/types.hpp>

namespace hq::consts {
    using namespace hal::literals;

    constexpr hal::pixel_t renderer_height { 768_px };

    constexpr hal::coord_t meters_per_pixel { 0.2_crd };
    constexpr hal::coord_t pixels_per_meter { 1.0_crd / meters_per_pixel };
}