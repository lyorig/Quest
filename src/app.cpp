#include <quest/app.hpp>

using namespace HQ;

using enum hal::video::renderer::flags;

App::App([[maybe_unused]] const Arguments& args)
    : video { m_context }
    , window { video, "HalodaQuest", { 0, 0 } } // Smallest possible size, essentially.
    , renderer { window, { accelerated, vsync } }
    , event { video.events }
{
    window.pos(hal::mouse::pos_abs().anchor(hal::anchor::center, window.size()));
    renderer.draw_color(hal::palette::black);
}

void App::Present()
{
    renderer.present();
}