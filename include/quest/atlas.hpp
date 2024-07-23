#pragma once

#include <halcyon/video.hpp>

// atlas.hpp:
// A texture atlas.
namespace hq {
    class atlas {
    public:
        atlas(hal::clref<hal::renderer> rnd, hal::pixel::point size);

        hal::pixel::rect add(hal::clref<hal::renderer> rnd, hal::ref<const hal::surface> surf);

        std::vector<hal::pixel::rect> free;

        hal::pixel::format  fmt;
        hal::target_texture tex;
    };
}
