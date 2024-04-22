#include <quest/state/main_menu.hpp>

using namespace HQ::State;

MainMenu::MainMenu(App& app)
    : m_theme { app.renderer.draw_color() }
    , m_currentTheme { false }
{
}

Type MainMenu::Update(App& app, hal::f64 elapsed)
{
    if (m_theme.Update(elapsed))
    {
        app.renderer.draw_color(m_theme.Value());
    }

    while (app.event.poll())
    {
        switch (app.event.event_type())
        {
            using enum hal::event::type;

        case quit_requested:
            return Type::Quit;

        case key_pressed:
            switch (app.event.keyboard().button())
            {
                using enum hal::keyboard::button;

            case T:
                m_theme.Start(SwitchTheme(), 1.0);

            default:
                break;
            }

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

hal::color MainMenu::SwitchTheme()
{
    constexpr hal::color colors[2] { hal::palette::red, hal::palette::black };
    m_currentTheme = !m_currentTheme;
    return colors[m_currentTheme];
}