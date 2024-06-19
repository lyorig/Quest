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
    : video { m_context }
    , audio { m_context }
    , img { hal::image::init_format::jpg }
    , window { video, consts::window_name, hal::tag::fullscreen }
    , renderer { window, { hal::renderer::flags::accelerated, cond_enum(hal::renderer::flags::vsync, a["-v"]) } } {
    renderer.blend(hal::blend_mode::blend);
}

void game::main_loop() {
    hal::timer timer;

    while (m_running) {
        m_delta = timer();
        timer.reset();

        collect_events();

        m_scenes.update(*this);

        renderer.present();
    }
}

void game::quit() {
    m_running = false;
}

void game::collect_events() {
    m_polled.clear();

    hal::event::handler h;

    while (video.events.poll(h)) {
        m_polled.push_back(h);
    }
}

const game::event_vector& game::polled() const {
    return m_polled;
}

delta_t game::delta() const {
    return m_delta;
}