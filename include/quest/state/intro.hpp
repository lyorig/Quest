#pragma once

#include <halcyon/ttf.hpp>
#include <halcyon/video.hpp>

#include <quest/state/base.hpp>

namespace HQ::State
{
    class Intro : public Base
    {
    public:
        Intro(hal::video::renderer& rnd, hal::ttf::context& ttf);

        Type Update(App& app, hal::f64 elapsed);
        void Draw(hal::video::renderer& rnd) const;

    private:
        hal::video::texture m_text;
        hal::coord_point    m_coord;
    };
}