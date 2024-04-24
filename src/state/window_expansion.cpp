#include <quest/state/window_expansion.hpp>

using namespace HQ::State;

WindowExpansion::WindowExpansion(App& app)
    : m_wndSize { app.window.size() }
    , m_wndPos { app.window.pos() }
    , m_drawCol { (app.renderer.draw_color(StartColor()), app.renderer.draw_color()) }
{
    constexpr Delta Time { 1.0 };

    m_wndSize.Start(app.video.displays[app.window.display_index()].size(), Time);
    m_wndPos.Start({ 0, 0 }, Time);
    m_drawCol.Start(EndColor(), Time);
}

Type WindowExpansion::Update(App& app, hal::f64 elapsed)
{
    if (m_wndSize.Update(elapsed))
    {
        app.window.size(m_wndSize.Value());

        m_wndPos.Update(elapsed);
        app.window.pos(m_wndPos.Value());

        m_drawCol.Update(elapsed);
        app.renderer.draw_color(m_drawCol.Value());

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

void WindowExpansion::Draw(hal::renderer& rnd) const
{
    static_cast<void>(rnd);
}