#include <quest/state/window_expansion.hpp>

using namespace HQ::State;

WindowExpansion::WindowExpansion(const hal::video::window& wnd, hal::pixel_point display_size)
    : m_wndSize { wnd.size() }
    , m_wndPos { wnd.pos() }
{
    constexpr Delta Time { 1.0 };

    m_wndSize.Start(display_size, Time);
    m_wndPos.Start({ 0, 0 }, Time);
}

Type WindowExpansion::Update(App& app, hal::f64 elapsed)
{
    if (m_wndSize.Update(elapsed))
    {
        m_wndPos.Update(elapsed);

        app.window.size(m_wndSize.Value());
        app.window.pos(m_wndPos.Value());

        while (app.event.poll())
        {
            switch (app.event.event_type())
            {
                using enum hal::event::type;

            case quit_requested:
                return Type::Quit;

            default:
                break;
            }
        }

        return Type::None;
    }

    // We're done.

    app.window.fullscreen(true);

    return Type::MainMenu;
}

void WindowExpansion::Draw(hal::video::renderer& rnd) const
{
    static_cast<void>(rnd);
}