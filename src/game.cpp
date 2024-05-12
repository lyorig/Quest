#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/state/rest.hpp>

using namespace HQ;

args::args(int argc, char** argv)
    : m_span { argv, static_cast<std::size_t>(argc) } {
}

bool args::operator[](std::string_view what) const {
    for (auto arg : m_span)
        if (hal::streq(arg, what.data()))
            return true;

    return false;
}

game::game([[maybe_unused]] args a)
    : m_video { m_context }
    , m_window { m_video.make_window("HalQuest", {}, { hal::window::flags::fullscreen_borderless }) }
    , m_renderer { m_window.make_renderer({ hal::renderer::flags::accelerated, hal::renderer::flags::vsync }) }
    , m_statics { state::console { m_ttf.load(hal::access("assets/m5x7.ttf"), 16) } }
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
                switch (m_event.keyboard().button()) {
                    using enum hal::keyboard::button;

                case F1:
                    std::get<state::console>(m_statics).Toggle(m_renderer);
                    break;

                default:
                    break;
                }
                break;

                // We aren't interested, but the current state might be.
            default:
                m_state->Process(m_event);
                break;
            }
        }

        if (state::Base* ptr = m_state->Update(delta); ptr != nullptr)
            m_state.reset(ptr);

        m_state->Draw(m_renderer);

        std::apply([this](const auto& obj) { obj.Draw(m_renderer); }, m_statics);

        m_renderer.present();
    }
}

void game::quit() {
    m_running = false;
}