#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/constants.hpp>

using namespace HQ;

namespace HQ::consts {
    constexpr std::string_view window_name { "HalQuest" };

    constexpr hal::keyboard::key console_toggle_bind { hal::keyboard::key::F1 };
}

args::args(int argc, char** argv)
    : m_span { argv, static_cast<std::size_t>(argc) } {
}

bool args::operator[](std::string_view what) const {
    for (auto arg : m_span) {
        if (what == arg) {
            return true;
        }
    }

    return false;
}

game::game(args)
    : m_video { m_context }
    , m_window { m_video.make_window(consts::window_name, hal::tag::fullscreen) }
    , m_renderer { m_window.make_renderer({ hal::renderer::flags::accelerated, hal::renderer::flags::vsync }) }
    , m_console { m_renderer, m_ttf }
    , m_event { m_video.events }
    , m_state { std::make_unique<state::main_menu>(m_renderer, m_ttf) } {
    using enum hal::renderer::flags;
    m_renderer.blend(hal::blend_mode::blend);
}

void game::main_loop() {
    hal::f64   delta;
    hal::timer timer;

    while (true) {
        delta = timer();
        timer.reset();

        while (m_event.poll()) {
            switch (m_event.event_type()) {
                using enum hal::event::type;

                // Handle universal events here.
            case quit_requested:
                return;

            case key_pressed:
                if (m_console.active()) {
                    if (m_console.process(m_event.keyboard().key(), m_video.events.keyboard.mod(), m_video.clipboard)) {
                        m_console.hide();
                        m_video.events.text_input_stop();
                    }

                    continue; // Nobody else gets incoming key events while the console is active.
                }

                switch (m_event.keyboard().key()) {
                    using enum hal::keyboard::key;

                case consts::console_toggle_bind:
                    m_console.show(m_renderer);
                    m_video.events.text_input_start();
                    break;

                case esc:
                    quit();
                    break;

                default:
                    break;
                }

                break;

            case text_input:
                m_console.process(m_event.text_input().text());
                break;

            default:
                break;
            }

            m_state->process(m_event);
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
    m_event.event_type(hal::event::type::quit_requested);
    m_event.push();
}