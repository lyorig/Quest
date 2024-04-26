#include <quest/app.hpp>

using namespace HQ;

using enum hal::renderer::flags;

App::App([[maybe_unused]] const Arguments& args)
    : video { m_context }
    , window { video.make_window("HalodaQuest", { 0, 0 }) } // Smallest possible size, essentially.
    , renderer { window.make_renderer({ vsync }) }
    , event { video.events }
{
    using namespace hal::literals;

    const hal::pixel_point dsz = video.displays[window.display_index()].size() / 2_px;

    window.pos(dsz.anchor(hal::anchor::center, window.size()));
    renderer.draw_color(hal::palette::black);
}

void App::Present()
{
    renderer.present();
}