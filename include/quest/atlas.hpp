#pragma once

#include <halcyon/video.hpp>

// atlas.hpp:
// A texture atlas.
namespace hq {
    class atlas {
    public:
        atlas(hal::clref<hal::renderer> rnd, hal::pixel::point size);

        hal::pixel::rect add(hal::clref<hal::renderer> rnd, hal::ref<const hal::surface> surf);

    private:
        std::vector<hal::pixel::rect> m_taken;

        hal::pixel::format  m_fmt;
        hal::target_texture m_tex;
    };
}
