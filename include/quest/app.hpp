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
        HAL_NO_SIZE hal::system::video video;

        hal::window   window;
        hal::renderer renderer;

        hal::event event;
    };
}