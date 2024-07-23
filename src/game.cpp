#include "halcyon/image.hpp"
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

args::args(int argc, char** argv)
    : m_span { const_cast<const char**>(argv), static_cast<std::size_t>(argc) } {
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
    , renderer { window, { hal::renderer::flag::accelerated, cond_enum(hal::renderer::flag::vsync, !a["--no-vsync"]) } }
    , atlas { renderer, { 128, 128 } }
    , timescale { 1.0 }
    , running { true }
    , m_screenshotPending { false } {
    renderer.blend(hal::blend_mode::blend);

    scenes.add(std::make_unique<scene::main_menu>(*this));
    scenes.add(std::make_unique<scene::console>(*this));
}

void game::main_loop() {
    hal::timer timer;

    while (running) {
        m_delta = timer();
        timer.reset();

        collect_events();

        scenes.update(*this);

        if (m_screenshotPending) {
            img.save(renderer.read_pixels(atlas.fmt), hal::image::save_format::png, "screenshot.png");
            m_screenshotPending = false;
        }

        renderer.present();
    }
}

void game::collect_events() {
    m_polled.clear();

    // Process global events here...
    while (video.events.poll(m_eventHandler)) {
        switch (m_eventHandler.kind()) {
            using enum hal::event::type;

        case quit_requested:
            running = false;
            break;

        case key_pressed:
            switch (m_eventHandler.keyboard().button()) {
                using enum hal::keyboard::button;

            case F12:
                m_screenshotPending = true;
                break;

            default:
                m_polled.push_back(m_eventHandler);
            }
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
    return m_delta * timescale;
}
