#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

using namespace HQ;

namespace HQ::consts {
    constexpr std::string_view window_name { "HalQuest" };
}

args::args(int argc, char** argv)
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
    , m_renderer { m_window, { hal::renderer::flags::accelerated, cond_enum(hal::renderer::flags::vsync, a["-v"]) } }
    , m_event { m_video.events }
    , m_console { m_renderer, m_ttf }
    , m_state { std::make_unique<state::main_menu>(m_renderer, m_ttf) } {
    m_renderer.blend(hal::blend_mode::blend);
}

void game::main_loop() {
    hal::f64   delta;
    hal::timer timer;

    while (true) {
        delta = timer();
        timer.reset();

        while (m_event.poll()) {
            switch (m_event.kind()) {
                using enum hal::event::type;

            // Handle universal events here.
            case quit_requested:
                return;

            case key_pressed:
                if (m_console.active()) {
                    if (m_console.process(m_event.keyboard().key(), m_video.events.keyboard.mod(), m_video.clipboard)) {
                        m_console.hide();
                        m_event.text_input_stop();
                    }

                    // Nobody else gets incoming key events while the console is active.
                    continue;
                }

                switch (m_event.keyboard().key()) {
                    using enum hal::keyboard::key;

                case F1:
                    m_console.show(m_renderer);
                    m_event.text_input_start();
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

        if (state::base* ptr = m_state->update(delta, m_renderer); ptr != nullptr)
            m_state.reset(ptr);

        if (m_console.active())
            m_console.update(m_renderer, delta);

        m_renderer.present();
    }
}

void game::quit() {
    m_event.kind(hal::event::type::quit_requested);
    m_event.push();
}