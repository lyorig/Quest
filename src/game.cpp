#include <quest/game.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/state/rest.hpp>

using namespace HQ;

Arguments::Arguments(int argc, char** argv)
    : m_span { argv, static_cast<std::size_t>(argc) } {
}

bool Arguments::operator[](std::string_view what) const {
    for (auto arg : m_span)
        if (hal::streq(arg, what.data()))
            return true;

    return false;
}

Game::Game([[maybe_unused]] Arguments args)
    : m_video { m_context }
    , m_window { m_video.make_window("HalQuest", {}, { hal::window::flags::fullscreen_borderless }) }
    , m_renderer { m_window.make_renderer({ hal::renderer::flags::accelerated, hal::renderer::flags::vsync }) }
    , m_event { m_video.events }
    , m_state { new State::MainMenu { m_renderer, m_ttf } } {
}

void Game::MainLoop() {
    hal::f64   delta;
    hal::timer timer;

    while (true) {
        delta = timer();
        timer.reset();

        while (m_event.poll()) {
            switch (m_event.event_type()) {
                using enum hal::event_handler::type;

                // Handle universal events here.
            case quit_requested:
                return;

                // We aren't interested, but the current state might be.
            default:
                m_state->Process(m_event);
                break;
            }
        }

        if (auto ptr = m_state->Update(delta); ptr != nullptr)
            m_state.reset(ptr);

        m_state->Draw(m_renderer);

        m_renderer.present();
    }
}