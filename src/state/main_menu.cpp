#include <quest/state/main_menu.hpp>

using namespace HQ::State;

MainMenu::MainMenu(const App& app)
    : m_wndSize { app.window.size() }
    , m_wndPos { app.window.pos() }
{
    constexpr hal::pixel_point NewSize { 1280, 720 };

    const hal::pixel_rect WndRect { hal::tag::as_size, app.video.displays[app.window.display_index()].size() };

    m_wndSize.Start(NewSize, 1.0);
    m_wndPos.Start(WndRect.anchor(hal::anchor::center, NewSize), 1.0);
}

Type MainMenu::Update(App& app, hal::f64 elapsed)
{
    if (m_wndSize.Update(elapsed))
    {
        app.window.size(m_wndSize.Value());
    }

    if (m_wndPos.Update(elapsed))
    {
        app.window.pos(m_wndPos.Value());
    }

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

void MainMenu::Draw(hal::video::renderer& rnd) const
{
    static_cast<void>(rnd);
}
