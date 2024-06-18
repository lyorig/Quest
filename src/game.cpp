#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

using namespace HQ;

namespace HQ::consts {
    constexpr std::string_view window_name { "HalQuest" };
}

args::args(int argc, const char** argv)
    : m_span { argv, static_cast<std::size_t>(argc) } {
}

bool args::operator[](std::string_view what) const {
    for (auto arg : m_span) {
        if (arg == what) {
            return true;
        }
    }

    return false;
}

game::game(args a)
    : m_video { m_context }
    , m_audio { m_context }
    , m_img { hal::image::init_format::jpg }
    , m_window { m_video, consts::window_name, hal::tag::fullscreen }
    , m_renderer { m_window, { hal::renderer::flags::accelerated, cond_enum(hal::renderer::flags::vsync, a["-v"]) } } {
    m_renderer.blend(hal::blend_mode::blend);
}

void game::main_loop() {
    hal::f64   delta;
    hal::timer timer;

    while (m_running) {
        delta = timer();
        timer.reset();

        while (m_video.events.poll(m_event)) {
            m_sceneMgr.process(m_event);
        }

        m_sceneMgr.update(delta, m_renderer);

        m_renderer.present();
    }
}

void game::quit() {
    m_event.kind(hal::event::type::quit_requested);
    m_video.events.push(m_event);
}