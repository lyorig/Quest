#pragma once

#include <halcyon/video.hpp>

// atlas.hpp:
// A texture atlas.
namespace hq {
    class atlas {
    public:
        atlas(hal::ref<hal::renderer> rnd, hal::pixel::point size);

        hal::pixel::rect add();

    private:
        hal::pixel::format  m_fmt;
        hal::target_texture m_tex;
    };
}
