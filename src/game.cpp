#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

using namespace HQ;

args::args(int argc, char** argv)
    : m_span { argv, static_cast<std::size_t>(argc) } {
}

bool args::operator[](std::string_view what) const {
    for (auto arg : m_span)
        if (what == arg)
            return true;

    return false;
}

game::game([[maybe_unused]] args a)
    : m_video { m_context }
    , m_window { m_video.make_window("HalQuest", {}, { hal::window::flags::fullscreen_borderless }) }
    , m_renderer { m_window.make_renderer({ hal::renderer::flags::accelerated, hal::renderer::flags::vsync }) }
    , m_console { state::console { m_ttf.load(hal::access("assets/m5x7.ttf"), 16) } }
    , m_event { m_video.events }
    , m_state { new state::main_menu { m_renderer, m_ttf } } {
    m_renderer.blend(hal::blend_mode::blend);
}

void game::main_loop() {
    hal::f64   delta;
    hal::timer timer;

    while (m_running) {
        delta = timer();
        timer.reset();

        while (m_event.poll()) {
            switch (m_event.event_type()) {
                using enum hal::event::type;

                // Handle universal events here.
            case quit_requested:
                m_running = false;
                break;

            case key_pressed:
                if (m_console.active()) {
                    m_console.process(m_event.keyboard().key(), m_video.events.keyboard.mod());
                }

                switch (m_event.keyboard().key()) {
                    using enum hal::keyboard::key;

                case F1:
                    if (m_console.toggle()) {
                        m_video.events.text_input_start();
                    } else {
                        m_video.events.text_input_stop();
                    }
                    break;

                default:
                    break;
                }
                break;

            case text_input:
                m_console.process(m_event.text_input().text()[0]);
                break;

            default:
                break;
            }
        }

        if (state::base* ptr = m_state->update(delta); ptr != nullptr)
            m_state.reset(ptr);

        m_state->draw(m_renderer);

        if (m_console.active())
            m_console.draw(m_renderer);

        m_renderer.present();
    }
}

void game::quit() {
    m_running = false;
}