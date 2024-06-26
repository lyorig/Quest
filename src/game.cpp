#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

#include <quest/scene/console.hpp>
#include <quest/scene/main_menu.hpp>

using namespace hq;

namespace hq::consts {
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
    , renderer { window, { hal::renderer::flags::accelerated, cond_enum(hal::renderer::flags::vsync, !a["--no-vsync"]) } }
    , m_running { true } {
    renderer.blend(hal::blend_mode::blend);

    scenes.add(std::make_unique<scene::main_menu>(*this));

    auto cons = std::make_unique<scene::console>(*this);

    if (!a["--console"]) {
        cons->deactivate();
    }

    scenes.add(std::move(cons));
}

void game::main_loop() {
    hal::timer timer;

    while (m_running) {
        m_delta = timer();
        timer.reset();

        collect_events();

        scenes.update(*this);

        renderer.present();
    }
}

void game::quit() {
    m_running = false;
}

void game::collect_events() {
    m_polled.clear();

    // Process global events here...
    while (video.events.poll(m_eventHandler)) {
        switch (m_eventHandler.kind()) {
            using enum hal::event::type;

        case quit_requested:
            quit();
            break;

        default:
            m_polled.push_back(m_eventHandler);
            break;
        }
    }
}

const game::event_vector& game::polled() const {
    return m_polled;
}

delta_t game::delta() const {
    return m_delta;
}