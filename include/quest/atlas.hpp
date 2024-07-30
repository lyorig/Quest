#pragma once

#include <halcyon/video.hpp>

// atlas.hpp:
// A texture atlas.
namespace hq {
    class atlas {
    public:
        enum class dim : bool {
            x,
            y
        };

        atlas(hal::lref<hal::renderer> rnd, hal::pixel::point size);

        hal::pixel::rect add(hal::lref<hal::renderer> rnd, hal::ref<const hal::surface> surf);

        std::vector<hal::pixel::rect> free;

        // The renderer's preferred format.
        hal::pixel::format fmt;

        // The atlas itself.
        hal::target_texture tex;

    private:
        hal::target_texture make_texture(hal::lref<hal::renderer> rnd, hal::pixel::point size);
    };
}
