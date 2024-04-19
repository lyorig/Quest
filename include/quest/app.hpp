#pragma once

#include <halcyon/event.hpp>
#include <halcyon/video.hpp>

#include <quest/arguments.hpp>

namespace HQ
{
    class App
    {
    public:
        App(const Arguments& args);

        void Present();

    private:
        hal::context m_context;

    public:
        HAL_NO_SIZE hal::ttf::context ttf;
        HAL_NO_SIZE hal::video::system video;

        hal::video::window   window;
        hal::video::renderer renderer;

        hal::event::handler event;
    };
}