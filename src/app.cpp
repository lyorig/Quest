#include <quest/app.hpp>

using namespace HQ;

using enum hal::video::renderer::flags;

App::App([[maybe_unused]] const Arguments& args)
    : video { m_context }
    , window { video, "HalodaQuest", { 640, 480 } }
    , renderer { window, { accelerated, vsync } }
    , event { video.events }
{
    renderer.draw_color(hal::palette::cyan);
}

void App::Present()
{
    renderer.present();
}